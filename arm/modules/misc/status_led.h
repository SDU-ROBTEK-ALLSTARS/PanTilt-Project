
#ifndef STATUS_LED_H
#define STATUS_LED_H

#define STATUS_LED_DELAY_TICKS  250
#define STATUS_LED_STACK_SIZE   configMINIMAL_STACK_SIZE
#define STATUS_LED_PRIORITY     tskIDLE_PRIORITY

void status_led_task(void *pvParameters);
BOOLEAN status_led_task_init(void);
void status_led_init_hw(void);


#endif /* STATUS_LED_H */
