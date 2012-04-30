
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Hardware definitions */
#include "inc/lm3s6965.h"

/* EMP course standard headers */
#include "inc/glob_def.h"
#include "inc/emp_type.h"

/* Driver module includes */
#include "comm/spi.h"

/* This modules header */
#include "spi_task.h"

/**
 * @file spi_task.c
 *
 * Implements SPI transmission and reception handling. The consumer task(s)
 * can get data from the spi_queue_in and provider task(s) puts data in
 * spi_queue_out. The spi_message_t data type is used as a container. The
 * spi_message_t.data field is what is actually transmitted on the hardware
 * line.
 */


/**
 * SPI transmit task (using FreeRTOS)
 *
 * @param pvParameters is a pointer to the task parameters (used when task
 * is created).
 *
 * This task checks spi_queue_out for messages waiting to be transmitted. If
 * there is any, the task will enable the "transmit FIFO half-empty or less"-
 * interrupt. The interrupt service routine spi_int_handler will then take
 * care of sending data in waiting in spi_queue_out.
 *
 * @note If task priority is higher than tskIDLE_PRIORITY, a specified time-to-
 * block should be specified with vTaskDelay.
 *
 * @return None (the task should never return).
 */
void spi_task_transmit(void *pvParameters)
{
  while(1)
  {
    if (uxQueueMessagesWaiting(spi_queue_out))
    {
      spi_interrupt_enable(SSI_IM_TXIM);
      taskYIELD();
    }
  }
}

/**
 * SPI interrupt service routine
 *
 * @param None
 *
 * The SPI interrupt service routine handles both receive and transmit
 * interrupts. Reception gets higher priority. The receive interrupts
 * are enabled when spi_task_init is called. The transmit interrupt is,
 * on the other hand, only enabled by spi_task_transmit provided there
 * is data waiting to be sent. It is important that ONLY spi_task_transmit
 * and spi_int_handler toggles the transmit interrupt.
 *
 * @return None
 */
void spi_int_handler(void)
{
  portBASE_TYPE higher_prio_task_woken = FALSE;
  INT32U status = spi_masked_int_status();

  if (status & (SSI_MIS_RTMIS | SSI_MIS_RXMIS))
  {
    /* The receive buffer is more than half-full, or the receive time-out has
    triggered. Both means there should be data in the receive hardware FIFO.
    This data is collected and sent to the spi_queue_in. */
    spi_interrupt_clear(SSI_ICR_RTIC);

    spi_message_t incoming_message;
    BOOLEAN read_more = TRUE;

    while (read_more)
    {
      if (spi_data_get(&incoming_message.data) == 1)
      {
        /* NOTE: No test is made here to see if there is room in the receive queue. */
        xQueueSendToBackFromISR(spi_queue_in, &incoming_message, &higher_prio_task_woken);
      }
      else
      {
        read_more = FALSE;
      }

      /* If a higher priority (than the one that was interrupted by this
      routine) task has woken, switch to that. */
      portEND_SWITCHING_ISR(higher_prio_task_woken);
    }
  }
  else if (status & SSI_MIS_TXMIS)
  {
    /* The transmit buffer is half-empty or less. If there is still data in the
    spi_queue_out, we wish to send it. Otherwise the transmit interrupt will be
    disabled (the spi_task_transmit can re-enable it when new data is queued) */
    spi_message_t outgoing_message;
    BOOLEAN write_more = TRUE;

    while (write_more)
    {
      if (xQueueReceiveFromISR(spi_queue_out, &outgoing_message, &higher_prio_task_woken) == pdTRUE)
      {
        /* There should be room in the transmit FIFO since the transmit interrupt
        brought us here */
        spi_data_put(outgoing_message.data);
      }
      else
      {
        spi_interrupt_disable(SSI_IM_TXIM);
        write_more = FALSE;
      }

      /* If a higher priority (than the one that was interrupted by this
      routine) task has woken, switch to that. */
      portEND_SWITCHING_ISR(higher_prio_task_woken);
    }
  }
  else
  {
    /* This condition should not occur. */
  }
}

/**
 * Initializer for SPI related tasks.
 *
 * @param None.
 *
 * This function creates the FreeRTOS SPI-transmit task and associated queues.
 * It also enables SPI receive interrupts.
 *
 * @return TRUE if the initialization was successful, FALSE otherwise.
 *
 * TODO: Set interrupt priorities?
 */
BOOLEAN spi_task_init(void)
{
  /* Public queues for in- and output */
  spi_queue_out = xQueueCreate(SPI_QUEUE_OUT_SIZE, sizeof(spi_message_t));
  spi_queue_in = xQueueCreate(SPI_QUEUE_IN_SIZE, sizeof(spi_message_t));

  /* Create transmit task */
  INT8U task_create_success = xTaskCreate(spi_task_transmit,
                                          (signed portCHAR *)"SPI_TRANSMIT",
                                          SPI_TASK_TRANSMIT_STACK_SIZE,
                                          NULL,
                                          SPI_TASK_TRANSMIT_PRIORITY,
                                          NULL);

  /* Enable the receive "FIFO half-full or more" and "receive timeout" interrupts */
  spi_interrupt_enable(SSI_IM_RXIM | SSI_IM_RTIM);

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
