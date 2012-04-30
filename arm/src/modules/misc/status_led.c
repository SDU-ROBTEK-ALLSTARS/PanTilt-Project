
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* EMP course standard headers */
#include "inc/glob_def.h"
#include "inc/emp_type.h"

/* Hardware definitions */
#include "inc/lm3s6965.h"

/* This modules header */
#include "status_led.h"

/**
 * Blinks the status LED every STATUS_LED_DELAY_TICKS
 *
 * @param None
 *
 */
void status_led_task(void *pvParameters)
{
  while(1)
  {
    GPIO_PORTF_DATA_R ^= 1;
    vTaskDelay(250);
  }
}

/**
 * Initializes the status LED task
 *
 * @param None.
 * @return TRUE if the initialization was successful, FALSE otherwise.
 */
BOOLEAN status_led_task_init(void)
{
  /* Create test task */
  INT8U task_create_success = xTaskCreate(status_led_task,
                                          (signed portCHAR *)"STATUS_LED",
                                          STATUS_LED_STACK_SIZE,
                                          NULL,
                                          STATUS_LED_PRIORITY,
                                          NULL);

  /* Make sure everything is constructed succesfully.
     The return value signals the outcome. */
  if (!task_create_success)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/**
 * Initializes the status LED hardware output
 *
 * @param None.
 * @return None.
 */
void status_led_init_hw(void)
{
  // Enable the GPIO port that is used for the on-board LED.
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;

  /* Wait a little */
  __asm("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop");

  // Set the direction as output.
  GPIO_PORTF_DIR_R |= 1;

  // Enable the GPIO pins for digital function.
  GPIO_PORTF_DEN_R |= 1;

  // Set pin high
  GPIO_PORTF_DATA_R |= 1;
}
