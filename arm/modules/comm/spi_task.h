
#ifndef SPI_TASK_H
#define SPI_TASK_H

#define SPI_TASK_TRANSMIT_PRIORITY    tskIDLE_PRIORITY
#define SPI_TASK_TRANSMIT_STACK_SIZE  configMINIMAL_STACK_SIZE
#define SPI_QUEUE_OUT_SIZE            16
#define SPI_QUEUE_IN_SIZE             SPI_QUEUE_OUT_SIZE

/* Register a type for a message to queue for sending via SPI. To queue a
message for sending, it must be placed in the spi_queue_in/out queues. */
typedef struct SPIMessageType
{
	INT16U data;
} spi_message_t;

xQueueHandle spi_queue_out, spi_queue_in;
BOOLEAN spi_task_init(void);

#endif /* SPI_TASK_H */
