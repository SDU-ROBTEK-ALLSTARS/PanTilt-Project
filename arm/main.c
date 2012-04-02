

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Project includes */
#include "sysctl/sysctl.h"

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
  sysctl_disable_global_int();
  sysctl_init_mclk();
  sysctl_enable_global_int();
}
