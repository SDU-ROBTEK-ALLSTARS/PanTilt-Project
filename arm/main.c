

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Module includes */
#include "sysctl/sysctl.h"
#include "comm/spi.h"


static void prvSetupHardware(void);

int main(void)
{
	prvSetupHardware();


	/* Start the scheduler. */
	vTaskStartScheduler();

  /* Will only get here if there was insufficient memory
  to create the idle task. */
	for( ;; );
	return 0;
}

void prvSetupHardware(void)
{
  sysctl_global_int_disable();
  sysctl_mclk_init();
  spi_init();
  sysctl_global_int_enable();
}
