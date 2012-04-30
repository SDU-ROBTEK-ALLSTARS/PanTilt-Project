
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* StellarisWare drivers */
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

/* EMP course standard headers */
#include "inc/glob_def.h"
#include "inc/emp_type.h"

/* Module includes */
#include "comm/spi.h"
#include "comm/spi_task.h"
#include "misc/status_led.h"

static void hardware_setup(void);

int main(void)
{
  /* Set up hardware */
  hardware_setup();

  /* Task initialization */
  if ( spi_task_init() &&
       status_led_task_init() )
  {
    vTaskStartScheduler();
  }

  while(1)
  {
    /* Will only get here if there was insufficient
       memory to create the idle task. */
  }
}

void hardware_setup(void)
{
  /* Drive at 50MHz crystal clock */
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
  SysCtlDelay(2);

  spi_init_hw();
  status_led_init_hw();

  IntMasterEnable();
}
