/* std lib */
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* StellarisWare drivers */
#include "inc/hw_types.h"
#include "inc/lm3s6965.h"
#include "driverlib/sysctl.h"

/* EMP course standard headers */
#include "glob_def.h"
#include "emp_type.h"

/* Module includes */
#include "comm/spi.h"
#include "comm/uart.h"
#include "comm/uart_protocol.h"
#include "utils/itoa.h"

/* This modules header */
#include "spi_test.h"

/**
 * @file spi_test.c
 *
 * The SPI test
 */

#define SPI_TEST_NUM_TASKS  8





static const char str_loopback_succes[] = " Data loop-back SUCCESS!\n";
static const char str_loopback_fail[] = " Data loop-back FAIL!\n";

static INT32U spi_test_num_loops;

static struct spi_test_id_t
{
  unsigned int id;
  xTaskHandle handle;
  void (*ptr)(void *);
  char name[configMAX_TASK_NAME_LEN];
} spi_test_task[SPI_TEST_NUM_TASKS];


/* Macro to easily expand into more functions/tasks */
#define SPI_TEST_TASK(TASK_NUM)                                                                \
  /* */                                                                                        \
  void spi_test_task_##TASK_NUM(void *params)                                                  \
  {                                                                                            \
    INT32U taskNameLen = strlen(spi_test_task[TASK_NUM].name);                                 \
    INT8U readBuf[2];                                                                          \
    INT8U writeBuf[2];                                                                         \
    INT32U result = 0;                                                                         \
                                                                                               \
    vTaskDelay((portTickType) (10 * TASK_NUM));                                                \
                                                                                               \
    uart_write((INT8U *) spi_test_task[TASK_NUM].name, taskNameLen, portMAX_DELAY);            \
    uart_write((INT8U *) " started\n", 10, portMAX_DELAY);                                     \
                                                                                               \
    for (INT32U i=0; i<spi_test_num_loops; i++)                                                \
    {                                                                                          \
      writeBuf[0] = (0x90 + TASK_NUM);                                                         \
      writeBuf[1] = (i + TASK_NUM);                                                            \
      spi_write_from_task(writeBuf, 2, portMAX_DELAY);                                         \
      spi_read_from_task(readBuf, 2, portMAX_DELAY);                                           \
                                                                                               \
      writeBuf[0] = (0x10 + TASK_NUM);                                                         \
      writeBuf[1] = 0x00;                                                                      \
      spi_write_from_task(writeBuf, 2, portMAX_DELAY);                                         \
      spi_read_from_task(readBuf, 2, portMAX_DELAY);                                           \
                                                                                               \
      if (readBuf[1] == (i + TASK_NUM))                                                        \
      {                                                                                        \
        result++;                                                                              \
      }                                                                                        \
    }                                                                                          \
                                                                                               \
    if (result == spi_test_num_loops)                                                          \
    {                                                                                          \
      uart_write((INT8U *) spi_test_task[TASK_NUM].name, taskNameLen, portMAX_DELAY);          \
      uart_write((INT8U *) str_loopback_succes, strlen(str_loopback_succes), portMAX_DELAY);   \
    }                                                                                          \
    else                                                                                       \
    {                                                                                          \
      uart_write((INT8U *) spi_test_task[TASK_NUM].name, taskNameLen, portMAX_DELAY);          \
      uart_write((INT8U *) str_loopback_fail, strlen(str_loopback_fail), portMAX_DELAY);       \
    }                                                                                          \
                                                                                               \
    uart_write((INT8U *) spi_test_task[TASK_NUM].name, taskNameLen, portMAX_DELAY);            \
    uart_write((INT8U *) " stopped\n", 10, portMAX_DELAY);                                     \
    while(1)                                                                                   \
    {                                                                                          \
    }                                                                                          \
  }

/* To register task pointer */
#define SPI_TEST_REG_PTR(TASK_NUM) /* */ \
  spi_test_task[TASK_NUM].ptr = &spi_test_task_##TASK_NUM;

SPI_TEST_TASK(0)
SPI_TEST_TASK(1)
SPI_TEST_TASK(2)
SPI_TEST_TASK(3)
SPI_TEST_TASK(4)
SPI_TEST_TASK(5)
SPI_TEST_TASK(6)
SPI_TEST_TASK(7)

void spi_test_start(uart_packet_t *packet)
{
  char number[4] = {'\0'};

  /* Set number of loops */
  spi_test_num_loops = packet->data[0];

  /* Signal start and confirm nr of loops */
  itoa(spi_test_num_loops, number, 10);
  uart_write((INT8U *) "Starting SPI test (", 19, portMAX_DELAY);\
  uart_write((INT8U *) number, sizeof(number), portMAX_DELAY);
  uart_write((INT8U *) " loops)\n", 8, portMAX_DELAY);\

  /* Fill in task addresses */
  SPI_TEST_REG_PTR(0)
  SPI_TEST_REG_PTR(1)
  SPI_TEST_REG_PTR(2)
  SPI_TEST_REG_PTR(3)
  SPI_TEST_REG_PTR(4)
  SPI_TEST_REG_PTR(5)
  SPI_TEST_REG_PTR(6)
  SPI_TEST_REG_PTR(7)

  /* Create tasks */
  for (int i=0; i<SPI_TEST_NUM_TASKS; i++)
  {
    if (spi_test_task[i].ptr != NULL)
    {
      spi_test_task[i].id = i;

      strncpy(spi_test_task[i].name, "SPI_TEST_", configMAX_TASK_NAME_LEN);
      itoa(i, number, 10);
      strncat(spi_test_task[i].name, number, strlen(number));

      xTaskCreate(spi_test_task[i].ptr, (signed portCHAR *) spi_test_task[i].name, (configMINIMAL_STACK_SIZE + 40), NULL, tskIDLE_PRIORITY, spi_test_task[i].handle);
    }
  }
}

BOOLEAN spi_test_init(void)
{
  return uart_protocol_register_handler(&spi_test_start, UART_PACKET_TYPE_SET, 1);
}

