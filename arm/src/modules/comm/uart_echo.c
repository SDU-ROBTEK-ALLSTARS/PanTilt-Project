
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* EMP course headers */
#include "glob_def.h"
#include "emp_type.h"

/* Modules */
#include "comm/uart.h"
#include "comm/uart_protocol.h"

/* This modules header */
#include "uart_echo.h"

/**
 * @file uart_echo.c
 *
 * Registers a handler to echo <data> back through the UART device
 *
 * The UART package is:
 *
 * Byte#             0          |              1            |     3-4      |    ..
 *        ----------------------------------------------------------------------------
 *        UART_ECHO_PACKET_TYPE | UART_ECHO_INSTRUCTION_NUM | <datalength> |  <data>
 *
 */


/**
 * UART echo protocol handler
 */
void uart_echo(uart_packet_t *p_packet)
{
  if (p_packet->datalength > 0)
  {
    uart_write((INT8U *) p_packet->data, (INT32U) p_packet->datalength, portMAX_DELAY);
  }
}

BOOLEAN uart_echo_init(void)
{
  return uart_protocol_register_handler(&uart_echo, UART_ECHO_PACKET_TYPE, UART_ECHO_INSTRUCTION_NUM);
}
