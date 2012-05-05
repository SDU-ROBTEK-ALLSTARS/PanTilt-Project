
#ifndef SPI_H
#define SPI_H

#define SPI_CONF_BITRATE       2000000
#define SPI_CONF_DATA_WIDTH    8
#define SPI_NUM_MAX_USERTASKS  16

void spi_config_hw(void);
BOOLEAN spi_init(void);
INT32S spi_write_from_task(const INT8U *buf, INT32U nbytes, portTickType ticks_to_block);
INT32S spi_read_from_task(INT8U *buf, INT32U nbytes, portTickType ticks_to_block);

#endif /* SPI_H */
