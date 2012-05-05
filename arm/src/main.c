
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
#include "test/comm/spi_test.h"

static void hardware_setup(void);

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
      uart_echo_init() &&
      spi_via_uart_init())
  {
    IntMasterEnable();
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
  SysCtlDelay(3);

  spi_config_hw();
  uart_init_hw();
  status_led_init_hw();
}
