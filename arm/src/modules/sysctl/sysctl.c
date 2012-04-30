

#include "inc/lm3s6965.h"
#include "sysctl.h"

/**
 * Enables interrupts globally
 */
void sysctl_global_int_enable()
{
	__asm("cpsie i");
}

/**
 * Disables interrupts globally
 */
void sysctl_global_int_disable()
{
  // disable interrupts.
  __asm("cpsid i");
}

/**
 * Initializes master clock to 50 MHz
 */
void sysctl_mclk_init()
{
	int dummy;

	// Step 1
	// Set BYPASS bit
	SYSCTL_RCC_R |= SYSCTL_RCC_BYPASS;
	// Clear USESYS bits
	SYSCTL_RCC_R &= ~(SYSCTL_RCC_USESYSDIV);
	// Enable Main osc.
	SYSCTL_RCC_R |= SYSCTL_RCC_MOSCDIS;

	// Step 2
	// Clear PLL lock flag
	SYSCTL_RIS_R = ~(SYSCTL_RIS_PLLLRIS);
	// Set XTAL value, first clear bits then set
	SYSCTL_RCC_R &= ~(SYSCTL_RCC_XTAL_M);
	SYSCTL_RCC_R |= SYSCTL_RCC_XTAL_8MHZ;
	// Set OSCSRC, first clear bits then set
	SYSCTL_RCC_R &= ~(SYSCTL_RCC_OSCSRC_M);
	SYSCTL_RCC_R |= SYSCTL_RCC_OSCSRC_MAIN;
	// Clear PWRDN
	SYSCTL_RCC_R &= ~(SYSCTL_RCC_PWRDN);

	// delay
	for (dummy = 200000; dummy >0; dummy--);

	// Step 3
	// Set SYSDIV, first clear bits then set
	// Set to 50 Mhz.
	SYSCTL_RCC_R &= ~(SYSCTL_RCC_SYSDIV_M);
	SYSCTL_RCC_R |= SYSCTL_RCC_SYSDIV_3;
	// Set USESYS bit
	SYSCTL_RCC_R |= SYSCTL_RCC_USESYSDIV;

	// Step 4
	// wait for PLL lock
	while(!(SYSCTL_RIS_R & SYSCTL_RIS_PLLLRIS));

	// Step 5
	// Clear BYPASS bit
	SYSCTL_RCC_R &= ~(SYSCTL_RCC_BYPASS);

	// delay
	for (dummy = 200000; dummy >0; dummy--);

}
