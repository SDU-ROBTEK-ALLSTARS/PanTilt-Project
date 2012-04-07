

#ifndef SPI_TASK_H
#define SPI_TASK_H

extern void spi_gatekeeper(void *pvParameters);
xQueueHandle spi_queue_out, spi_queue_in;

#endif /* SPI_TASK__H */
