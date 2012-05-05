
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* EMP course headers */
#include "inc/glob_def.h"
#include "inc/emp_type.h"

/* Modules */
#include "comm/uart.h"
#include "comm/uart_protocol.h"

#include "uart_echo.h"

void uart_echo(uart_packet_t *p_packet)
{
  INT32U result;

  if (p_packet->datalength > 0)
  {
    result = 0;

    /* Keep trying to send until great succes is obtained */
    while (result < p_packet->datalength)
    {
      result += uart_write((INT8U *) p_packet->data, ((INT32U) p_packet->datalength - result), (portTickType) 0xFF);
    }
  }
}

BOOLEAN uart_echo_init(void)
{
  return uart_protocol_register_handler(&uart_echo, UART_ECHO_TYPE, UART_ECHO_INSTR_NR);
}
