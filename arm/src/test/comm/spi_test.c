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

/* This modules header */
#include "spi_test.h"

/**
 * @file spi_test.c
 *
 * The SPI test
 */



/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
char* itoa(int value, char* result, int base)
{
  // check that the base if valid
  if (base < 2 || base > 36)
  {
    *result = '\0';
    return result;
  }

  char* ptr = result, *ptr1 = result, tmp_char;
  int tmp_value;

  do
  {
    tmp_value = value;
    value /= base;
    *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
  }
  while (value);

  // Apply negative sign
  if (tmp_value < 0) *ptr++ = '-';
  *ptr-- = '\0';
  while(ptr1 < ptr)
  {
    tmp_char = *ptr;
    *ptr--= *ptr1;
    *ptr1++ = tmp_char;
  }
  return result;
}


#define SPI_TEST_NUM_TASKS 1



static const char str_loopback_succes[] = " Data loop-back SUCCESS!\n";
static const char str_loopback_fail[] = " Data loop-back FAIL!\n";


static struct spi_test_id_t
{
  unsigned int id;
  xTaskHandle handle;
  void (*ptr)(void *);
  char name[configMAX_TASK_NAME_LEN];
} spi_test_task[SPI_TEST_NUM_TASKS];


/* Task */
#define SPI_TEST_TASK(TASK_NUM)                                                                                  \
  void spi_test_task_##TASK_NUM(void *params)                                                                    \
  {                                                                                                              \
    const char write_buf[] = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz";          \
    char read_buf[sizeof(write_buf)];                                                                            \
    int num = 20;                                                                                                \
                                                                                                                 \
                                                                                                                 \
    uart_write((INT8U *) spi_test_task[TASK_NUM].name, strlen(spi_test_task[TASK_NUM].name), portMAX_DELAY);     \
    uart_write((INT8U *) " started\n", 9, portMAX_DELAY);                                                        \
                                                                                                                 \
                                                                                                                 \
    spi_write_from_task((INT8U *) write_buf, num, portMAX_DELAY);                                                \
                                                                                                                 \
                                                                                                                 \
    spi_read_from_task((INT8U *) read_buf, num, portMAX_DELAY);                                                  \
                                                                                                                 \
    if (strncmp(write_buf, read_buf, num) == 0)                                                                  \
    {                                                                                                            \
      uart_write((INT8U *) spi_test_task[TASK_NUM].name, strlen(spi_test_task[TASK_NUM].name), portMAX_DELAY);   \
      uart_write((INT8U *) str_loopback_succes, strlen(str_loopback_succes), portMAX_DELAY);                     \
    }                                                                                                            \
    else                                                                                                         \
    {                                                                                                            \
      uart_write((INT8U *) spi_test_task[TASK_NUM].name, strlen(spi_test_task[TASK_NUM].name), portMAX_DELAY);   \
      uart_write((INT8U *) str_loopback_fail, strlen(str_loopback_fail), portMAX_DELAY);                         \
                                                                                                                 \
      uart_write((INT8U *) "\n", 1, portMAX_DELAY);                                                              \
      uart_write((INT8U *) write_buf, num, portMAX_DELAY);                                                       \
      uart_write((INT8U *) "\n", 1, portMAX_DELAY);                                                              \
      uart_write((INT8U *) read_buf, num, portMAX_DELAY);                                                        \
    }                                                                                                            \
                                                                                                                 \
    vTaskDelete(NULL);                                                                                           \
  }


/* Register task pointer */
#define SPI_TEST_REG_PTR(TASK_NUM) \
  spi_test_task[TASK_NUM].ptr = &spi_test_task_##TASK_NUM;


SPI_TEST_TASK(0)
//SPI_TEST_TASK(1)
//SPI_TEST_TASK(2)

void spi_test_start(void)
{
  char name_number[4];

  /* Enable SPI loopback mode */
  SSI0_CR1_R |= SSI_CR1_LBM;

  /* Fill in task addresses */
  SPI_TEST_REG_PTR(0)
  //SPI_TEST_REG_PTR(1)
  //SPI_TEST_REG_PTR(2)

  /* Create tasks */
  for (int i=0; i<SPI_TEST_NUM_TASKS; i++)
  {
    if (spi_test_task[i].ptr != NULL)
    {
      spi_test_task[i].id = i;

      strncpy(spi_test_task[i].name, "SPI_TASK_", configMAX_TASK_NAME_LEN);
      itoa(i, name_number, 10);
      strncat(spi_test_task[i].name, name_number, sizeof(name_number));

      xTaskCreate(spi_test_task[i].ptr, (signed portCHAR *) spi_test_task[i].name, (configMINIMAL_STACK_SIZE + 40), NULL, tskIDLE_PRIORITY, spi_test_task[i].handle);
    }
  }
}

BOOLEAN spi_test_init(void)
{
  return uart_protocol_register_handler(&spi_test_start, UART_PACKET_TYPE_SET, 1);
}

