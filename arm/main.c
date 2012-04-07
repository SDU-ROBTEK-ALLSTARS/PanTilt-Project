
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* EMP course standard headers */
#include "inc/glob_def.h"
#include "inc/emp_type.h"

/* Driver module includes */
#include "sysctl/sysctl.h"
#include "comm/spi.h"

/* Tasks */
#include "comm/spi_task.h"

static void hardware_setup(void);

int main(void)
{
  /* Set up hardware */
  hardware_setup();

  /* Task initialization */
  BOOLEAN success = spi_gatekeeper_init();
  while(!success)
  {
    /* Stop here if not successful */
  }

  /* Start the scheduler. */
  vTaskStartScheduler();

  while(1)
  {
    /* Will only get here if there was insufficient
       memory to create the idle task. */
  }
}

void hardware_setup(void)
{
  sysctl_global_int_disable();
  sysctl_mclk_init();
  spi_init_hw();
  sysctl_global_int_enable();
}
