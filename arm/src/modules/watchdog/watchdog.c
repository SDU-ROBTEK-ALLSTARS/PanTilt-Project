/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* StellarisWare drivers */

#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/watchdog.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

/* EMP course standard headers */
#include "glob_def.h"
#include "emp_type.h"

/* Module includes */
#include "comm/spi.h"
#include "fpga_registers.h"

/* This modules header */
#include "watchdog.h"

/**
 * @file watchdog.c
 *
 * Runs a task which resets the watchdog timer. If the scheduler crashes so
 * that the the WD interrupt is no longer reset, it will trigger a hardware
 * reset. In addition, tasks which are supposed to run regularly can "check in"
 * via wd_kick_from_task() to let the WD know they are still running. This adds
 * an additional level of error detection.
 *
 * The WD kicker task is also responsible to signal to the FPGA that the MCU
 * is still alive. If the FPGA doesn't receive it's keep-alive signal, it too
 * will reset.
 *
 */

#define WD_NUM_TASKS                      10
#define WD_TIMER_VAL                      (configCPU_CLOCK_HZ * 0.3)
#define WD_SLOWEST_CHECKIN_ALLOWED_TICKS  (300 / portTICK_RATE_MS)

struct wd_singletask
{
  xTaskHandle task_handle;
  INT32U tick_since_sched_start;
};

struct wd_control
{
  struct wd_singletask task_list[WD_NUM_TASKS];
  BOOLEAN test_ok;
  xSemaphoreHandle mutex;
};

/* Struct with tasks the WD should regularly check */
static struct wd_control tasks_to_check;

/* Binary semaphore to be given by the ISR */
static xSemaphoreHandle wd_task_sem;

/**
 * Watchdog-kick function to call from tasks to let the WD know they are
 * still running.
 */
void wd_kick_from_task(void)
{
  xTaskHandle caller_taskhandle, current_taskhandle;
  INT32U i;

  caller_taskhandle = xTaskGetCurrentTaskHandle();

  if ((xSemaphoreTake(tasks_to_check.mutex, portMAX_DELAY) == pdTRUE) && (caller_taskhandle != NULL))
  {
    /* Find the calling task in the list */
    i = 0;
    do
    {
      current_taskhandle = tasks_to_check.task_list[i].task_handle;

      if (current_taskhandle == caller_taskhandle)
      {
        /* The calling task was found in the array */
        tasks_to_check.task_list[i].tick_since_sched_start = (INT32U) xTaskGetTickCount();
        break;
      }
      else if (current_taskhandle == NULL)
      {
        /* The calling task was not found, but there is room to give it a spot */
        tasks_to_check.task_list[i].task_handle = caller_taskhandle;
        tasks_to_check.task_list[i].tick_since_sched_start = (INT32U) xTaskGetTickCount();
        break;
      }
      else
      {
        /* This is another tasks' array element; move on */
        i++;
      }

    } while (i < WD_NUM_TASKS);


    xSemaphoreGive(tasks_to_check.mutex);
  }
}

/**
 * Watchdog ISR
 */
void wd_int_handler(void)
{
  portBASE_TYPE higher_prio_task_woken = pdFALSE;

  /* We don't take the mutex here (there is no API for it, when it's from
   * an ISR). We should be ok though, as FALSE is a singular value (0). */
  if (tasks_to_check.test_ok == TRUE)
  {
    WatchdogIntClear(WATCHDOG0_BASE);
    tasks_to_check.test_ok = FALSE; /* This must be set true again by wd_kicker_task */
  }

  xSemaphoreGiveFromISR(wd_task_sem, &higher_prio_task_woken);
  portEND_SWITCHING_ISR(higher_prio_task_woken);
}

/**
 * Watchdog kicker task
 *
 * This task blocks on the binary semaphore given by the ISR. When that is
 * given, it indicates that it is time to run a system check. If all is Ok
 * the WD interrupt+timer is reset. If not, the WD interrupt is not reset
 * and next time the ISR fires it will hard-reset the processor.
 */
void wd_kicker_task(void *params)
{
  INT32U current_tick_count;
  INT32U result, i;
  INT8U buf[2];

  spi_register_task(NULL);

  while (1)
  {
    if (xSemaphoreTake(wd_task_sem, portMAX_DELAY) == pdTRUE)
    {
      /* Watchdog has had it's first timeout. We must check stuffs OK-ness
       * and reset the WD interrupt before it runs out a second time and
       * asserts hardware reset. We don't give back a binary semaphore. */

      if (xSemaphoreTake(tasks_to_check.mutex, portMAX_DELAY) == pdTRUE)
      {
        current_tick_count = (INT32U) xTaskGetTickCount();

        /* Loop through tasks in the task_list and see if they have "checked in"
         * lately. */
        result = 0;

        for (i=0; i<WD_NUM_TASKS; i++)
        {
          if (tasks_to_check.task_list[i].task_handle == NULL)
          {
            break;
          }
          else if ((INT32S) (tasks_to_check.task_list[i].tick_since_sched_start - current_tick_count) < (INT32S) WD_SLOWEST_CHECKIN_ALLOWED_TICKS)
          {
            result++;
          }
        }

        /* Reset WD, if all is OK */
        if (result == i)
        {
          tasks_to_check.test_ok = TRUE;

          /* We have to perform a read-modify-write operation to "kick" the FPGA
           * by writing */
          buf[0] = ADDRESS_AUX_TO_ARM;
          buf[1] = 0x00;
          spi_write_from_task(buf, 2, portMAX_DELAY);
          spi_read_from_task(buf, 2, portMAX_DELAY);

          buf[0] = ADDRESS_AUX_FROM_ARM | 0x80;
          buf[1] |= ~(buf[1] & ((1 << AUX_REG_WATCHDOG_BIT_0) | (1 << AUX_REG_WATCHDOG_BIT_1)));
          spi_write_from_task(buf, 2, portMAX_DELAY);
          spi_read_from_task(buf, 2, portMAX_DELAY); /* Dummy read just to flush buffer */
        }
        else
        {
          tasks_to_check.test_ok = FALSE;
        }

        xSemaphoreGive(tasks_to_check.mutex);
      }
    }
  }
}

/**
 * Configures the watchdog timer
 */
BOOLEAN wd_init(void)
{
  taskENTER_CRITICAL();

  SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);
  SysCtlDelay(3);

  /* If locked, unlock */
  if (WatchdogLockState(WATCHDOG0_BASE) == true)
  {
    WatchdogUnlock(WATCHDOG0_BASE);
  }

  IntEnable(INT_WATCHDOG);

  /* Stall the WD timer when processor is stopped by the debugger in
   * debug-builds */
  #ifdef DEBUG
  WatchdogStallEnable(WATCHDOG0_BASE);
  #endif

  WatchdogReloadSet(WATCHDOG0_BASE, (unsigned long) WD_TIMER_VAL);
  WatchdogResetEnable(WATCHDOG0_BASE);
  WatchdogEnable(WATCHDOG0_BASE);
  WatchdogLock(WATCHDOG0_BASE);

  taskEXIT_CRITICAL();

  vSemaphoreCreateBinary(wd_task_sem);

  tasks_to_check.mutex = xSemaphoreCreateMutex();
  tasks_to_check.test_ok = FALSE;
  for (INT32U i=0; i<WD_NUM_TASKS; i++)
  {
    tasks_to_check.task_list[i].task_handle = NULL;
  }

  if (xTaskCreate(wd_kicker_task, (signed portCHAR *) "Watchdog", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL) &&
      wd_task_sem &&
      tasks_to_check.mutex)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
