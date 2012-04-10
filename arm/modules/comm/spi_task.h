
#ifndef SPI_TASK_H
#define SPI_TASK_H

#define SPI_GATEKEEPER_TASK_PRIORITY   (tskIDLE_PRIORITY + 1)
#define SPI_GATEKEEPER_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define SPI_QUEUE_OUT_SIZE             16
#define SPI_QUEUE_IN_SIZE              SPI_QUEUE_OUT_SIZE

/* Register a type for a message to queue for sending
   via SPI. Makes it a lot easier to change data types
   or size if need be.
   It is important to notice the queues used pass
   spi_message_structs around! */
typedef struct
{
	INT16U data;
} spi_message_t;

extern BOOLEAN spi_gatekeeper_task_init(void);
extern void spi_gatekeeper_task(void *pvParameters);
xQueueHandle spi_queue_out, spi_queue_in;

/* Below are test tasks mainly for debugging */
extern BOOLEAN spi_gatekeeper_task_test_init(void);
extern void spi_gatekeeper_task_test(void *pvParameters);

#endif /* SPI_TASK__H */
