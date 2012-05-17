
#ifndef SPI_H
#define SPI_H

#define SPI_RX_TASK_PRIORITY       (tskIDLE_PRIORITY + 1)
#define SPI_TX_TASK_PRIORITY       (tskIDLE_PRIORITY + 1)
#define SPI_BITRATE                2000000
#define SPI_DATA_WIDTH             8
#define SPI_NUM_MAX_USERTASKS      16
#define SPI_INTERN_QUEUE_IN_SIZE   32
#define SPI_PUBLIC_QUEUE_SIZE      32

void spi_config_hw(void);
BOOLEAN spi_init(void);
BOOLEAN spi_register_task(xTaskHandle taskhandle);
INT32S spi_write_from_task(const INT8U *buf, INT32U nbytes, portTickType ticks_to_block);
INT32S spi_read_from_task(INT8U *buf, INT32U nbytes, portTickType ticks_to_block);

#endif /* SPI_H */
