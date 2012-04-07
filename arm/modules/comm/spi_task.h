
#ifndef SPI_TASK_H
#define SPI_TASK_H

#define SPI_GATEKEEPER_PRIORITY   (tskIDLE_PRIORITY + 1)
#define SPI_GATEKEEPER_STACK_SIZE (configMINIMAL_STACK_SIZE)
#define SPI_QUEUE_OUT_SIZE        16
#define SPI_QUEUE_IN_SIZE         SPI_QUEUE_OUT_SIZE

extern BOOLEAN spi_gatekeeper_init(void);
extern void spi_gatekeeper(void *pvParameters);
xQueueHandle spi_queue_out, spi_queue_in;

#endif /* SPI_TASK__H */
