
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* EMP course standard headers */
#include "glob_def.h"
#include "emp_type.h"

/* Module includes */
#include "comm/spi.h"
#include "comm/uart.h"
#include "comm/uart_protocol.h"

/* This modules header */
#include "uart_to_spi.h"

/**
 * @file uart_to_spi.c
 *
 * Registers a handler and task to forward <data> on the UART to the SPI
 * connection.
 *
 * The UART package is:
 *
 * Byte#             0            |              1              |     3-4      |   ..
 *        ------------------------------------------------------------------------------
 *        UART_TO_SPI_PACKET_TYPE | UART_TO_SPI_INSTRUCTION_NUM | <datalength> | <data>
 *
 *
 */

static xQueueHandle uart_to_spi_queue;

/**
 * UART to SPI protocol handler
 */
void uart_to_spi_write(uart_packet_t *packet)
{
  for (INT32U i=0; i<packet->datalength; i++)
  {
    xQueueSendToBack(uart_to_spi_queue, &packet->data[i], portMAX_DELAY);
  }
}

/**
 * UART to SPI task
 */
void uart_to_spi_task(void *param)
{
  INT8U data;

  while(1)
  {
    xQueueReceive(uart_to_spi_queue, &data, portMAX_DELAY);
    spi_write_from_task(&data, 1, portMAX_DELAY);

    spi_read_from_task(&data, 1, portMAX_DELAY);
    uart_write(&data, 1, portMAX_DELAY);
  }
}

BOOLEAN uart_to_spi_init(void)
{
  if (xTaskCreate(uart_to_spi_task, (signed portCHAR *) "UART_-_SPI", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL) &&
      (uart_to_spi_queue = xQueueCreate(8, sizeof(INT8U))) &&
      uart_protocol_register_handler(uart_to_spi_write, UART_TO_SPI_PACKET_TYPE, UART_TO_SPI_INSTRUCTION_NUM))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
