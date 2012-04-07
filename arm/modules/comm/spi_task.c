
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* EMP course standard headers */
#include "inc/glob_def.h"
#include "inc/emp_type.h"

/* Driver module includes */
#include "comm/spi.h"

/* This modules header */
#include "spi_task.h"

void spi_gatekeeper(void *pvParameters)
{


  while(1)
  {

  }
}

BOOLEAN spi_gatekeeper_init(void)
{
  /* Create queues for in- and output */
  spi_queue_out = xQueueCreate(SPI_QUEUE_OUT_SIZE, SPI_DATA_SIZE);
  spi_queue_in = xQueueCreate(SPI_QUEUE_IN_SIZE, SPI_DATA_SIZE);

  /* Create gatekeeper task */
  INT8U task_create_success = xTaskCreate(spi_gatekeeper,
                                          (signed portCHAR *)"SPI_GATEKEEPER",
                                          SPI_GATEKEEPER_STACK_SIZE,
                                          NULL,
                                          SPI_GATEKEEPER_PRIORITY,
                                          NULL);

  /* Make sure everything is constructed succesfully.
     Return value signals the outcome. */
  if (!spi_queue_in || !spi_queue_out || !task_create_success)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}
