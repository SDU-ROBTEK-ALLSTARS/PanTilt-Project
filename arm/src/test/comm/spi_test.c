
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* EMP course standard headers */
#include "inc/glob_def.h"
#include "inc/emp_type.h"

/* Module includes */
#include "comm/spi.h"
#include "comm/uart.h"
#include "comm/uart_protocol.h"

/* This modules header */
#include "spi_test.h"

/**
 * @file spi_test.c
 *
 * The SPI test initializes
 */
//
//
//
//
//
//
//
//
//&&
//      spi_via_uart_init()
//
//
//#ifdef DEBUG
//#include "test/comm/spi_test.h"
//#endif
//
//
//  #if SPI_LOOPBACK_ON != 0
//    SSI0_CR1_R |= SSI_CR1_LBM;
//  #endif
//


static xQueueHandle spi_via_uart_queue_set, spi_via_uart_queue_get;

void spi_via_uart_task(void *params)
{
  INT8U data;

  while(1)
  {
    if (xQueueReceive(spi_via_uart_queue_set, &data, (portTickType) 0) == pdTRUE)
    {
      spi_write_from_task(&data, 1, portMAX_DELAY);
    }

    if (spi_read_from_task(&data, 1, (portTickType) 0xFF) == 1)
    {
      xQueueSendToBack(spi_via_uart_queue_get, &data, portMAX_DELAY);
    }
  }
}

void spi_via_uart_set(uart_packet_t *p_packet)
{
  for (INT8U i=0; i<p_packet->datalength; i++)
  {
    xQueueSendToBack(spi_via_uart_queue_set, &p_packet->data[i], portMAX_DELAY);
  }
}

void spi_via_uart_get(uart_packet_t *p_packet)
{
  INT8U data;

  while (xQueueReceive(spi_via_uart_queue_get, &data, (portTickType) 0) == pdTRUE)
  {
    uart_write(&data, 1, portMAX_DELAY);
  }
}

BOOLEAN spi_via_uart_init(void)
{
  if (xTaskCreate(spi_via_uart_task, (signed portCHAR *)"SPI_UART_TEST", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL) &&
      (spi_via_uart_queue_set = xQueueCreate(32, sizeof(INT8U))) &&
      (spi_via_uart_queue_get = xQueueCreate(32, sizeof(INT8U))) &&
      uart_protocol_register_handler(&spi_via_uart_set, UART_PACKET_TYPE_SET, 10) &&
      uart_protocol_register_handler(&spi_via_uart_get, UART_PACKET_TYPE_GET, 11))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

BOOLEAN spi_test_init(void)
{
  return TRUE;
}
