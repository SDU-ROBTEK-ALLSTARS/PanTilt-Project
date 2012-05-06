
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* StellarisWare drivers */
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

/* EMP course standard headers */
#include "inc/glob_def.h"
#include "inc/emp_type.h"

/* Module includes */
#include "comm/spi.h"
#include "comm/uart.h"
#include "comm/uart_protocol.h"
#include "comm/uart_echo.h"
#include "misc/status_led.h"

#ifdef DEBUG
#include "test/comm/spi_test.h"
#include "test/sys/runtimestats.h"
#endif /* DEBUG */

void hardware_setup(void);

int main(void)
{
  IntMasterDisable();

  /* Set up hardware */
  hardware_setup();

  /* Module initialization */
  if (spi_init() &&
      uart_init_task() &&
      uart_protocol_init_task() &&
      status_led_task_init() &&
      uart_echo_init()
      #ifdef DEBUG
      && spi_test_init()
      && runtimestats_init()
      #endif /* DEBUG */
      )
  {
    IntMasterEnable();
    vTaskStartScheduler();
  }

  while(1)
  {
    /* Will only get here if initialization went wrong. */
  }

  return 1;
}

void hardware_setup(void)
{
  /* Drive at 50MHz crystal clock */
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
  SysCtlDelay(3);
  spi_config_hw();
  uart_init_hw();
  status_led_init_hw();
  #ifdef DEBUG
  timer0_config_hw();
  #endif /* DEBUG */
}
