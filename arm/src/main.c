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

/* Global headers */
#include "glob_def.h"
#include "emp_type.h"
#include "os.h"
#include "maindefs.h"

/* Module includes */
#include "comm/spi.h"
#include "comm/uart.h"
#include "comm/uart_protocol.h"
#include "comm/uart_echo.h"
#include "comm/uart_to_spi.h"
#include "itc/itc.h"
#include "leds/leds.h"
#include "leds/status_led.h"
#include "lcd16x2/lcd.h"
#include "dreh/dreh.h"
#include "numpad/numpad.h"
#include "adc/adc.h"
#include "menu/menu.h"
#include "control/control.h"
#include "itc/parameter_updater.h"
#include "stepresponse/stepresponse.h"
#include "watchdog/watchdog.h"

#ifdef DEBUG
#include "test/comm/spi_test.h"
#include "test/sys/runtimestats.h"
#endif /* DEBUG */

void hardware_setup(void);
void init_semaphores(void);
void init_queues(void);

xSemaphoreHandle sem_lcd_buffer;
xSemaphoreHandle sem_common_pins;
xSemaphoreHandle sem_queue;
xSemaphoreHandle sem_event;
xSemaphoreHandle sem_counter;
xSemaphoreHandle sem_state;
xSemaphoreHandle sem_parameter;
xTaskHandle task_handles[10];

int main(void)
{
  IntMasterDisable();

  /* Set up hardware */
  hardware_setup();
  init_semaphores();
  init_queues();

  /* Module initialization */
  if (spi_init() &&
      uart_init_task() &&
      uart_protocol_init_task() &&
      status_led_task_init() &&
      uart_echo_init() &&
      par_updater_init() &&
      (xTaskCreate(dreh_task,     NAME("Dreh"),    DEFAULT_STACK, NULL, PRIORITY_LOW,   &task_handles[DREH_T])) == pdPASS &&
      (xTaskCreate(lcd_task,      NAME("LCD"),     DEFAULT_STACK, NULL, PRIORITY_LOW,   &task_handles[LCD_T])) == pdPASS &&
      (xTaskCreate(menu_task,     NAME("Menu"),    LARGE_STACK,   NULL, PRIORITY_LOW,   &task_handles[MENU_T])) == pdPASS &&
      (xTaskCreate(numpad_task,   NAME("Numpad"),  DEFAULT_STACK, NULL, PRIORITY_LOW,   &task_handles[NUMPAD_T])) == pdPASS &&
      (xTaskCreate(control_task,  NAME("Control"), DEFAULT_STACK, NULL, PRIORITY_HIGH,  &task_handles[CONTROL_T])) == pdPASS &&
      (xTaskCreate(blink_task,    NAME("Blink"),   DEFAULT_STACK, NULL, PRIORITY_LOW,   &task_handles[BLINK_T])) == pdPASS &&
      uart_to_spi_init() &&
      step_response_init() &&
      wd_init()
      #ifdef DEBUG
      && spi_test_init()
      && runtimestats_init()
      #endif /* DEBUG */
     )
  {
    vTaskStartScheduler();
  }

  while (1)
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
  display_init();
  init_leds();
  init_dreh();
  key_init();
  init_adc();
  #ifdef DEBUG
  timer0_config_hw();
  #endif /* DEBUG */
}

void init_semaphores(void)
{
  sem_lcd_buffer  = xSemaphoreCreateMutex();
  sem_common_pins = xSemaphoreCreateMutex();
  sem_queue     = xSemaphoreCreateMutex();
  sem_event     = xSemaphoreCreateMutex();
  sem_counter   = xSemaphoreCreateMutex();
  sem_state     = xSemaphoreCreateMutex();
  sem_parameter = xSemaphoreCreateMutex();
}

void init_queues(void)
{

}
