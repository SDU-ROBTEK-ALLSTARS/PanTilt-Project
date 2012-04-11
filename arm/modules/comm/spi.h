

#ifndef SPI_H
#define SPI_H

/* Configurable SPI settings */
#define SPI_PRESCALE    10 /* Even integer from 2 to 254 */
#define SPI_CLKRATE     4  /* Integer from 0 to 255 */
                           /* SPI clock frequency is =
                              MCLK / (SPI_PRESCALE * (1 + SPI_CLKRATE)) */
#define SPI_DATA_SIZE   8  /* Integer from 4 to 16 defining size of one frame */
#define SPI_LOOPBACK    1

void spi_init_hw(void);
INT8U spi_data_put(INT16U data);
INT8U spi_data_get(INT16U *p_data);
void spi_interrupt_enable(INT32U interrupt_id);
void spi_interrupt_disable(INT32U interrupt_id);
INT32U spi_masked_int_status(void);
void spi_interrupt_clear(INT32U interrupt_ids);
BOOLEAN spi_out_full(void);
BOOLEAN spi_out_empty(void);
BOOLEAN spi_in_full(void);
BOOLEAN spi_in_empty(void);
BOOLEAN spi_busy(void);

#endif /* SPI_H */
