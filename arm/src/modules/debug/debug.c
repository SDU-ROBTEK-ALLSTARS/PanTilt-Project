/* std lib */
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* StellarisWare drvs */
#include "driverlib/debug.h"

/* EMP course headers */
#include "glob_def.h"
#include "emp_type.h"

/* Module includes */
#include "comm/uart.h"

/* This modules header */
#include "debug.h"

/**
 * Error function used by the Stellaris driver library.
 */
void __error__(char *filename, unsigned long line)
{
  while(1)
  {
  }
}

/**
 * Stack overflow hook (declared extern from freeRTOS tasks.c)
 */
void vApplicationStackOverflowHook(xTaskHandle *p_taskhandle, signed portCHAR *taskname)
{
  vTaskSuspend(*p_taskhandle);
  uart_write((INT8U *) "Stack overflow:", 15, portMAX_DELAY);
}
