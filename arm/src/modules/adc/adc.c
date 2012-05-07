/*****************************************************************************
* University of Southern Denmark
* Robotics Engineering Spring 2012
* Pan Tilt Project
*
* MODULENAME.: adc
*****************************************************************************/

/***************************** Include files *******************************/
#include "adc.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"

/***********************   Function implementations   **********************/
void init_adc(void)
{
	// Enable clock
	SYSCTL_RCGC0_R |= SYSCTL_RCGC0_ADC /* | SYSCTL_RCGC0_ADCSPD1M*/;
	SysCtlDelay(3);

	// Disable ADC while initializing
	ADC_ACTSS_R = 0;

	// Set trigger to manual
	ADC_EMUX_R = 0;

	// Set input for sample sequence
	ADC_SSMUX1_R |= ADC_CHANNEL;

	// Set end of sequence to last sample
	ADC_SSCTL1_R |= ADC_SSCTL1_END0;

	// Enable the sample sequencer 1 (ss1)
	ADC_ACTSS_R |= ADC_ACTSS_ASEN1;

	// Start AD Conversion sequence on ss1.
	ADC_PSSI_R |= ADC_PSSI_SS1;
}

int get_adc(void)
{
	int result;

	// Get result of latest ADC
	result = ADC_SSFIFO1_R;

	// Check for FIFO overflow
	if(ADC_OSTAT_R);

	// Check for FIFO underflow
	if(ADC_USTAT_R);

	// Start AD Conversion sequence o ss1.
	ADC_PSSI_R |= ADC_PSSI_SS1;

	return result;
}
