/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* StellarisWare drivers */
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"

/* EMP course standard headers */
#include "glob_def.h"
#include "emp_type.h"

/* Module includes */
#include "comm/uart.h"
#include "comm/uart_protocol.h"
#include "menu/menu.h"

/* This modules header */
#include "stepresponse.h"

/**
 * @file stepresponse.c
 *
 * Registers a handler to capture step-response data by utilizing the
 * begin_step() and end_step() provided by menu.h. A high-frequency timer
 * is used to keep track of time more precisely. Data is saved in a two
 * arrays as sets of (time,position).
 *
 * num_measurements and task_delay_ms specifies how often (period time)
 * the task should run and how many measurements should be taken. Those
 * arguments are configurable via. the UART packet (so the receiver can
 * know how many bytes should be expected returned.
 *
 * Memory is allocated dynamically, so the number of measurements which
 * can be saved is limited to how much memory is available in RAM.
 * When the task has run and sent the log data through UART, it frees
 * the previously allocated memory and shuts itself down.
 */

static xTaskHandle step_response_log_task_handle;
volatile static unsigned long _timer1_ticks;
static INT32U *timeData;
static INT32S *posData;
static INT32U num_measurements;
static INT32U task_delay_ms;
static xQueueHandle logMutex;

void timer1_setup(void)
{
  taskDISABLE_INTERRUPTS();

  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  SysCtlDelay(3);

  TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

  TimerLoadSet(TIMER1_BASE, TIMER_A, (unsigned long) (configCPU_CLOCK_HZ / 1000));

  IntPrioritySet(INT_TIMER1A, (unsigned char)(1 << 5));
  TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
  IntEnable(INT_TIMER1A);

  TimerEnable(TIMER1_BASE, TIMER_A);

  taskENABLE_INTERRUPTS();
}

/*
 * ISR forward-declared in startup.c
 */
void timer1_int_handler(void)
{
  TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
  _timer1_ticks++;
}

/*
 * Task to handle data-logging. It is not run unless specifically asked to.
 */
void step_response_log_task(void *params)
{
  if (xSemaphoreTake(logMutex, (portTickType) 0xFFF) == pdTRUE)
  {
    INT32U i = 0;
    timer1_setup();

    parameter(PUSH,TILT_PWM_P, 0x8000);
    parameter(PUSH,PAN_PWM_P, 0);

    /* Switching out this task a bit to let others register the new settings */
    vTaskDelay((portTickType) 1);

    _timer1_ticks = 0UL;

    while (1)
    {
      /* Save data. We won't be prempted while logging each data couple */
      taskENTER_CRITICAL();
      timeData[i] = (INT32U) _timer1_ticks;
      posData[i] = (INT32S) parameter(POP, TILT_POSITION_P);
      taskEXIT_CRITICAL();

      i++;

      if (i == num_measurements)
      {
        /* Stop the motors and disable the timer again */
        parameter(PUSH,TILT_PWM_P, 0);
        parameter(PUSH,PAN_PWM_P, 0);

        /* Switching out this task a bit to let others register the new settings */
        vTaskDelay((portTickType)(10 / portTICK_RATE_MS));

        TimerIntDisable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
        TimerDisable(TIMER1_BASE, TIMER_A);

        /* Suspending tasks and writing directly to the UART hardware FIFO
         * here. Optimal solution would be a printf implementation instead? */
        vTaskSuspendAll();

        for (INT32U j=0; j<i; j++)
        {
          UARTCharPut(UART0_BASE, (unsigned char)(timeData[j] >> 0));
          UARTCharPut(UART0_BASE, (unsigned char)(timeData[j] >> 8));
          UARTCharPut(UART0_BASE, (unsigned char)(timeData[j] >> 16));
          UARTCharPut(UART0_BASE, (unsigned char)(timeData[j] >> 24));

          UARTCharPut(UART0_BASE, (unsigned char)(posData[j] >> 0));
          UARTCharPut(UART0_BASE, (unsigned char)(posData[j] >> 8));
          UARTCharPut(UART0_BASE, (unsigned char)(posData[j] >> 16));
          UARTCharPut(UART0_BASE, (unsigned char)(posData[j] >> 24));
        }

        /* Resume all other tasks again, but kill this one and free the memory
         * it's been using. The idle task is responsible for deleting this
         * task after vTaskDelete has been called. */
        xTaskResumeAll();
        i = 0;
        vPortFree(timeData);
        vPortFree(posData);
        timeData = NULL;
        posData = NULL;
        xSemaphoreGive(logMutex);
        vTaskDelete(NULL);
      }
      else
      {
        vTaskDelay((portTickType)(task_delay_ms / portTICK_RATE_MS));
      }
    }
  }
}

/*
 * UART protocol handler to begin the step-response logging.
 * If wrong arguements, it does nothing.
 */
void step_response_trigger(uart_packet_t *p_packet)
{
  portBASE_TYPE task_create_return;

  if (xSemaphoreTake(logMutex, (portTickType) 0xFFF) == pdTRUE)
  {
    if (p_packet->datalength == 8)
    {
      /* Get the packet arguments */
      num_measurements = (INT32U) p_packet->data[0];
      num_measurements |= (INT32U) (p_packet->data[1] << 8);
      num_measurements |= (INT32U) (p_packet->data[2] << 16);
      num_measurements |= (INT32U) (p_packet->data[3] << 24);
      task_delay_ms = (INT32U) p_packet->data[4];
      task_delay_ms |= (INT32U) (p_packet->data[5] << 8);
      task_delay_ms |= (INT32U) (p_packet->data[6] << 16);
      task_delay_ms |= (INT32U) (p_packet->data[7] << 24);

      /* Allocate memory for log data */
      timeData = (INT32U *) pvPortMalloc(num_measurements * sizeof(INT32U));
      posData = (INT32S *) pvPortMalloc(num_measurements * sizeof(INT32S));
      task_create_return = xTaskCreate(step_response_log_task,
                                       (signed portCHAR *) "STEP_LOGGER",
                                       configMINIMAL_STACK_SIZE,
                                       NULL,
                                       (tskIDLE_PRIORITY + 1),
                                       &step_response_log_task_handle);

      /* Check if the allocations were succesful */
      if (timeData == NULL || posData == NULL || task_create_return != pdPASS)
      {
        uart_write((INT8U *) "ERRNOMEM\n", 9, portMAX_DELAY);

        if (task_create_return == pdPASS)
        {
          vTaskDelete(step_response_log_task_handle);
        }
      }
    }
    xSemaphoreGive(logMutex);
  }
}

BOOLEAN step_response_init(void)
{
  logMutex = xSemaphoreCreateMutex();
  return uart_protocol_register_handler(&step_response_trigger, UART_PACKET_TYPE_SET, 3);
}
