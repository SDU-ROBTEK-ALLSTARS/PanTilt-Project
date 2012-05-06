
/* std libc */
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* StellarisWare drivers */
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"

/* EMP course standard headers */
#include "inc/glob_def.h"
#include "inc/emp_type.h"

/* Module includes */
#include "comm/uart.h"
#include "comm/uart_protocol.h"

/* This modules header */
#include "runtimestats.h"

/*
 * Forward declared in startup.c
 */
volatile unsigned long __timer0_ticks;

/*
 * Forward declared in startup.c
 */
void timer0_int_handler(void)
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	__timer0_ticks++;
}

void runtimestats_print(void)
{
  char *buf;
  buf = (char *) pvPortMalloc(uxTaskGetNumberOfTasks() * 40);

  if (buf != NULL)
  {
    vTaskGetRunTimeStats((signed char*) buf);
    uart_write((INT8U *) buf, strlen((char *) buf), portMAX_DELAY);
    vPortFree(buf);
  }
}

BOOLEAN runtimestats_init(void)
{
  __timer0_ticks = 0UL;
  return uart_protocol_register_handler(&runtimestats_print, UART_PACKET_TYPE_GET, RUNTIMESTATS_REQUEST_INSTRUCTION_NUMBER);
}

void timer0_config_hw(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	SysCtlDelay(3);

  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

  TimerLoadSet(TIMER0_BASE, TIMER_A, (unsigned long) TIMER0_CONF_FREQUENCY);

  IntPrioritySet(INT_TIMER0A, (unsigned char) (1 << 5));
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  IntEnable(INT_TIMER0A);

  TimerEnable(TIMER0_BASE, TIMER_A);
}
