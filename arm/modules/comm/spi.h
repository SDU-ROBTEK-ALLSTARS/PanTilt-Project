

#ifndef SPI_H
#define SPI_H

/* Configurable SPI settings */
#define SPI_PRESCALE  10 /* Even integer from 2 to 254 */
#define SPI_CLKRATE   4  /* Integer from 0 to 255 */
                         /* SPI clock frequency is =
                            MCLK / (SPI_PRESCALE * (1 + SPI_CLKRATE)) */

#define SPI_DATA_SIZE 8  /* Integer from 4 to 16 defining size of one frame */

extern void spi_init_hw(void);
extern INT8U spi_data_put(INT16U data);
extern INT8U spi_data_get(INT16U *p_data);
extern BOOLEAN spi_out_full(void);
extern BOOLEAN spi_out_empty(void);
extern BOOLEAN spi_in_full(void);
extern BOOLEAN spi_in_empty(void);
extern BOOLEAN spi_busy(void);

#endif /* SPI_H */
