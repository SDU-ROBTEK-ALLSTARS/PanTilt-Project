
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

/**
 * SPI gatekeeper task (using FreeRTOS)
 *
 * @param pvParameters is a pointer to the task parameters (used when task
 * is created)
 *
 * This tasks acts as the doorway between software I/O SPI queues and the
 * hardware buffers. The gatekeeper task is the only task permitted
 * direct access to the SPI hardware!
 *
 * @return None (the task should never return)
 */
void spi_gatekeeper_task(void *pvParameters)
{
  /* Initialize a message struct. Only one is used as temporary storage
     for both in- and outgoing messages: The FreeRTOS queue implementation
     copies by value. */
  spi_message_t message;

  while(1)
  {
    /* Wait for an item from the SPI outgoing queue. The task BLOCKS here
       while waiting! */
    xQueueReceive(spi_queue_out, &message, portMAX_DELAY);

    /* Write the message to the hardware FIFO */
    while (spi_data_put(message.data) != 1)
    {
      /* Keep trying until we succeed. */
    }

    /* When data has been sent, incoming data is certain... */
    while (spi_data_get(&message.data) != 1)
    {
      /* Loop until data is picked up
         TODO: This may be a bad solution: What is the delay between
         transmitting until data is written to the incoming buffer? */
    }

    /* Place a copy of the incoming data in the in-queue. The task will
       BLOCK here until there is space in the in-queue! */
    xQueueSendToBack(spi_queue_in, &message, portMAX_DELAY);
  }
}

/**
 * Initializer for the SPI gatekeeper task.
 *
 * @param None.
 *
 * This function creates the FreeRTOS SPI-gatekeeper task and associated queues.
 *
 * @return TRUE if the initialization was successful, FALSE otherwise.
 */
BOOLEAN spi_gatekeeper_task_init(void)
{
  /* Create queues for in- and output */
  spi_queue_out = xQueueCreate(SPI_QUEUE_OUT_SIZE, sizeof(spi_message_t));
  spi_queue_in = xQueueCreate(SPI_QUEUE_IN_SIZE, sizeof(spi_message_t));

  /* Create gatekeeper task */
  INT8U task_create_success = xTaskCreate(spi_gatekeeper_task,
                                          (signed portCHAR *)"spi_gatekeeper_task",
                                          spi_gatekeeper_task_STACK_SIZE,
                                          NULL,
                                          spi_gatekeeper_task_PRIORITY,
                                          NULL);

  /* Make sure everything is constructed succesfully.
     The return value signals the outcome. */
  if (!spi_queue_in || !spi_queue_out || !task_create_success)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/**
 * Self-test task
 */
void spi_gatekeeper_task_test(void *pvParameters)
{
  spi_message_t test_message;

  while(1)
  {
    test_message.data = 0x88;
    xQueueSendToBack(spi_queue_out, &test_message, portMAX_DELAY);

    vTaskDelay(10);
  }
}

/**
 * Initializes the self-test task
 */
BOOLEAN spi_gatekeeper_task_test_init(void)
{
  /* Create test task */
  INT8U task_create_success = xTaskCreate(spi_gatekeeper_task_test,
                                          (signed portCHAR *)"SPI_GK_TEST",
                                          configMINIMAL_STACK_SIZE,
                                          NULL,
                                          spi_gatekeeper_task_PRIORITY,
                                          NULL);

  /* Make sure everything is constructed succesfully.
     The return value signals the outcome. */
  if (!task_create_success)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}
