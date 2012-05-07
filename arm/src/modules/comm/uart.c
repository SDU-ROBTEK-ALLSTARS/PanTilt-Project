
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* StellarisWare drivers */
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"

/* EMP course headers */
#include "glob_def.h"
#include "emp_type.h"

/* This modules header */
#include "uart.h"

/**
 * @file uart.c
 */

extern xQueueHandle uart_queue_in;
static struct
{
  xQueueHandle queue;
  xSemaphoreHandle mutex;
} uart_out;

/**
 * Writes the content of BUFFER (up to LENGTH) to the UART output queue
 *
 * @param buffer is a pointer to a data byte-array
 * @param length is the number of bytes to send
 * @param ticks_to_block is how many ticks the function can block for
 *
 * If ticks_to_block is specified, the function should always return
 * result = length. Otherwise, it will return the number of bytes written
 * sucessfully. The user must then decide what action to take.
 *
 * @return the number of bytes succesfully written to the queue.
 */
INT32U uart_write(const INT8U *buffer, INT32U length, portTickType ticks_to_block)
{
  INT32U bytes_written = 0;

  if (xSemaphoreTake(uart_out.mutex, ticks_to_block) == pdTRUE)
  {
    for (INT32U i=0; i<length; i++)
    {
      if (xQueueSendToBack(uart_out.queue, &buffer[i], ticks_to_block) == errQUEUE_FULL)
      {
        break;
      }
      else
      {
        bytes_written++;
      }
    }
    xSemaphoreGive(uart_out.mutex);
  }

  return bytes_written;
}

/**
 * UART ISR (declared in startup.c)
 *
 * Triggers on RX FIFO half-full or more, and RX timeout. Empties the hardware
 * FIFO into the uart_queue_in.
 */
void uart0_isr(void)
{
  UARTIntClear(UART0_BASE, UART_INT_RT);

  portBASE_TYPE higher_prio_task_woken = FALSE;
  INT8U received_char = (INT8U) UARTCharGetNonBlocking(UART0_BASE);
  xQueueSendToBackFromISR(uart_queue_in, &received_char, &higher_prio_task_woken);
  portEND_SWITCHING_ISR(higher_prio_task_woken);
}

/**
 * UART transmitter task
 */
void uart_task_transmit(void *pvParameters)
{
  INT8U char_to_transmit;

  while(1)
  {
    /* If there is room in the UART transmit FIFO, attempt to get a value from
    the queue of data to transmit. If there is data in the queue, transmit it.
    If not, the task blocks while waiting for data.
    Mutex is NOT taken here: We only send one byte at a time, and synchronization
    is already taken care of. */
    xQueueReceive(uart_out.queue, &char_to_transmit, portMAX_DELAY);
    UARTCharPut(UART0_BASE, char_to_transmit);
  }
}

/**
 * Configures the FreeRTOS units for UART operation
 */
BOOLEAN uart_init_task(void)
{
  /* Queues for in- and output */
  uart_out.queue = xQueueCreate(UART_QUEUE_OUT_SIZE, sizeof(INT8U));
  uart_out.mutex = xSemaphoreCreateMutex();
  uart_queue_in = xQueueCreate(UART_QUEUE_IN_SIZE, sizeof(INT8U));

  /* Create transmit task */
  INT8U task_create_success = xTaskCreate(uart_task_transmit,
                                          (signed portCHAR *)"UART_TX",
                                          UART_TASK_TRANSMIT_STACK_SIZE,
                                          NULL,
                                          UART_TASK_TRANSMIT_PRIORITY,
                                          NULL);

  /* Make sure everything is constructed succesfully.
  The return value signals the outcome. */
  if (!uart_out.queue || !uart_queue_in || !task_create_success)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/**
 * Configures the UART hardware settings
 */
void uart_init_hw(void)
{
  /* Enable periphs */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlDelay(3);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  SysCtlDelay(3);

  /* Set GPIO A0 and A1 as UART pins */
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  /* 115200 baud rate, 8-N-1 operation */
  UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                       UART_CONFIG_PAR_NONE));

  /* Enable UART FIFO */
  UARTFIFOEnable(UART0_BASE);

  /* Interrupt on receive FIFO half-full (default) or receive time-out */
  IntEnable(INT_UART0);
  UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

  /* Set interrupt priority to one level LOWER than
  configMAX_SYSCALL_INTERRUPT_PRIORITY so that we can still use FreeRTOS API
  functions from within it.
  Remember: Higher value => lower priority. */
  IntPrioritySet(INT_UART0, configMAX_SYSCALL_INTERRUPT_PRIORITY + (unsigned char) (2 << 5));

  /* Enable the UART */
  UARTEnable(UART0_BASE);
}
