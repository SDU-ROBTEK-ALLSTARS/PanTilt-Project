

#include "lm3s6965.h"
#include "spi.h"


/* Sets 1 MHz SPI clock freq: */
#define SPI_PRESCALE   10  /* Even integer from 2 to 254 */
#define SPI_CLKRATE    4   /* Integer from 0 to 255 */
                           /* SPI clock frequency is =
                              MCLK / (SPI_PRESCALE * (1 + SPI_CLKRATE)) */

#define SPI_DATA_SIZE  SSI_CR0_DSS_8

/**
 * Sets up the SSI interface as follows:
 *
 * - Freescale SPI mode
 * - Master
 * - Clock as defined by SPI_PRESCALE and SPI_CLKRATE
 *
 */
void spi_init(void)
{
  /* Enable periphiral clock */
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_SSI0;

  /* Wait a little */
  __asm("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop");

  /* Make sure the serial port is disabled */
  SSI0_CR1_R &= ~(SSI_CR1_SSE);

  /* I am Master! */
  SSI0_CR1_R &= ~(SSI_CR1_SOD | SSI_CR1_MS | SSI_CR1_LBM);

  /* Set clock prescaler */
  SSI0_CPSR_R |= (SSI_CPSR_CPSDVSR_M & SPI_PRESCALE);

  /* Set clock rate,
     Data is captured on the first clock edge transition,
     SSIClk is HIGH when idle
     Freescale SPI frame format */
  SSI0_CR0_R |= ( ((SSI_CR0_SCR_M & (SPI_CLKRATE << 8))
                   | SSI_CR0_SPO | SSI_CR0_FRF_MOTO | SPI_DATA_SIZE)
                  & ~SSI_CR0_SPH );

  /* FOR TESTING: LOOPBACK MODE! */
  SSI0_CR1_R |= SSI_CR1_LBM;

  /* Enable SPI serial port */
  SSI0_CR1_R |= SSI_CR1_SSE;
}
