
/* Hardware definitions */
#include "inc/lm3s6965.h"

/* EMP course standard headers */
#include "inc/glob_def.h"
#include "inc/emp_type.h"

/* This modules header */
#include "spi.h"

/**
 * @file spi.c
 *
 */


/**
 * Sets up the SSI hardware interface as follows:
 *
 * - Freescale SPI mode
 * - Master
 * - Clock as defined by SPI_PRESCALE and SPI_CLKRATE
 * - Data frame size as defined by SPI_DATA_SIZE
 */
void spi_init_hw(void)
{
  /* Enable periphiral clock */
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_SSI0;

  /* Wait a little */
  __asm("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop");

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

  /* Enable the SPI interrupt in NVIC */
  NVIC_EN0_R |= (1 << 7);

  /* FOR TESTING: LOOPBACK MODE! */
  if (SPI_LOOPBACK_ON)
  {
    SSI0_CR1_R |= SSI_CR1_LBM;
  }

  /* Enable SPI serial port */
  SSI0_CR1_R |= SSI_CR1_SSE;
}

/**
 * Get the masked interrupt status
 *
 * @param None
 *
 * The return value can be ANDed with SSI_MIS_xxxx value to check
 * individual interrupt assertions.
 *
 * @return INT32U masked interrupt status
 */
INT32U spi_masked_int_status(void)
{
  return SSI0_MIS_R;
}

/**
 * Un-masks the interrupt passed as an argument
 *
 * @param INT32U value defined as SSI_IM_RXIM, SSI_IM_TXIM, SSI_IM_RORIM
 * or SSI_IM_RTIM in the hardware register definitions.
 *
 * @return None
 */
void spi_interrupt_enable(INT32U interrupt_ids)
{
  SSI0_IM_R |= interrupt_ids;
}

/**
 * Masks the interrupt passed as an argument
 *
 * @param INT32U value defined as SSI_IM_RXIM, SSI_IM_TXIM, SSI_IM_RORIM
 * or SSI_IM_RTIM in the hardware register definitions.
 *
 * @return None
 */
void spi_interrupt_disable(INT32U interrupt_ids)
{
  SSI0_IM_R &= ~(interrupt_ids);
}

/**
 * Clears the interrupt flag passed as an argument
 *
 * @param INT32U value defined as SSI_ICR_RORIC or SSI_ICR_RTIC in the
 * hardware register definitions.
 *
 * @return None
 */
void spi_interrupt_clear(INT32U interrupt_ids)
{
  SSI0_ICR_R &= ~(interrupt_ids);
}

/**
 * Puts data in the SPI hardware outgoing-FIFO.
 *
 * @param data is the data element to be sent.
 *
 * This function will attempt to write the supplied data elements to the
 * hardware transmit FIFO. The data elements can be maximum 16 bits wide,
 * but how much of the data is really being used is decided by SPI_DATA_SIZE.
 * If SPI_DATA_SIZE is shorter than the supplied data, the upper bits will
 * be discarded without notice.
 * The FIFO queue is automatically emptied as the transmission hardware
 * sends the data.
 *
 * @return 1 if write is succesful, 0 if not.
 */
INT8U spi_data_put(INT16U data)
{
  if (SSI0_SR_R & SSI_SR_TNF)
  {
    SSI0_DR_R = data;
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
 * Reads data from the SPI hardware receive-FIFO.
 *
 * @param p_data is a pointer where data should be stored.
 *
 * This function attempts to read a data element from the SPI in-going
 * hardware FIFO. If the buffer is empty, nothing is read and the
 * return value will be zero.
 *
 * @return 1 if an element is read succesfully, 0 if not.
 */
INT8U spi_data_get(INT16U *p_data)
{
  if (SSI0_SR_R & SSI_SR_RNE)
  {
    *p_data = SSI0_DR_R;
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
 * Checks if the outgoing FIFO is full.
 *
 * @param None.
 * @return TRUE if full, FALSE otherwise.
 */
BOOLEAN spi_out_full(void)
{
  if (SSI0_SR_R & SSI_SR_TNF)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/**
 * Checks if the outgoing FIFO is empty.
 *
 * @param None.
 * @return TRUE if empty, FALSE otherwise.
 */
BOOLEAN spi_out_empty(void)
{
  if (SSI0_SR_R & SSI_SR_TFE)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/**
 * Checks if the in-going FIFO is full.
 *
 * @param None.
 * @return TRUE if full, FALSE otherwise.
 */
BOOLEAN spi_in_full(void)
{
  if (SSI0_SR_R & SSI_SR_RFF)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/**
 * Checks if the in-going FIFO is empty.
 *
 * @param None.
 * @return TRUE if empty, FALSE otherwise.
 */
BOOLEAN spi_in_empty(void)
{
  if (SSI0_SR_R & SSI_SR_RNE)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/**
 * Checks if the SSI unit is busy.
 *
 * @param None.
 *
 * This function tests if the SSI interface is busy. Busy means a data frame
 * is currently being transmitted or received or that the transmit FIFO is
 * not empty.
 *
 * @return TRUE is busy, FALSE if idle.
 */
BOOLEAN spi_busy(void)
{
  if (SSI0_SR_R & SSI_SR_BSY)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}