
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* EMP course standard headers */
#include "glob_def.h"
#include "emp_type.h"

/* Module includes */
#include "comm/spi.h"
#include "itc/itc.h"
#include "fpga_registers.h"
#include "watchdog/watchdog.h"

/* This modules header */
#include "parameter_updater.h"

void param_updater_task(void *params)
{
  /*
  const INT8U parMap[] = {PAN_PWM_P,
                          TILT_PWM_P,
                          PAN_POSITION_P,
                          TILT_POSITION_P,
                          PAN_VELOCITY_P,
                          TILT_VELOCITY_P,
                          FREE_P};
  */
  INT8U transmitBuf[] = {(ADDRESS_DUTY_A_MSB | 0x80), 0x00,
                         (ADDRESS_DUTY_A_LSB | 0x80), 0x00,
                         (ADDRESS_DUTY_B_MSB | 0x80), 0x00,
                         (ADDRESS_DUTY_B_LSB | 0x80), 0x00,
                         ADDRESS_POS_A_MSB, ADDRESS_POS_A_LSB,
                         ADDRESS_POS_B_MSB, ADDRESS_POS_B_LSB,
                         ADDRESS_VEL_A_MSB, ADDRESS_VEL_A_LSB,
                         ADDRESS_VEL_B_MSB, ADDRESS_VEL_B_LSB,
                         /*(ADDRESS_AUX_FROM_ARM | 0x80), ADDRESS_AUX_TO_ARM,*/
                         0x00}; /* The 0x00 bytes sent are dummies */
  INT8U readBuf[sizeof(transmitBuf)]; /* The first byte returned is not used. */
  INT32S temp;

  /* Registers this task to use the SPI */
  spi_register_task(NULL);

  while(1)
  {
    /* Get new PWM values */
    temp = parameter(POP, PAN_PWM_P);
    transmitBuf[1] = (INT8U) (temp >> 8);
    transmitBuf[3] = (INT8U) temp;

    temp = parameter(POP, TILT_PWM_P);
    transmitBuf[5] = (INT8U) (temp >> 8);
    transmitBuf[7] = (INT8U) temp;

    /* Send array of addresses and new PWM vals. Read in the response in
     * the readBuf buffer. */
    spi_write_from_task(transmitBuf, sizeof(transmitBuf), portMAX_DELAY);
    spi_read_from_task(readBuf, sizeof(transmitBuf), portMAX_DELAY);

    /* Reading whole arrays seems to be working as inteded now. In case
     * that goes wrong, try this: */
    /*
    for (INT32U i=0; i<sizeof(transmitBuf); i++)
    {
      spi_write_from_task(&transmitBuf[i], 1, portMAX_DELAY);
      spi_read_from_task(&readBuf[i], 1, portMAX_DELAY);
    }
    */

    /* Update "our" values with the updated ones from the FPGA */
    parameter(PUSH, PAN_POSITION_P, (INT32S) ((readBuf[9] << 8) | readBuf[10]));
    parameter(PUSH, TILT_POSITION_P, (INT32S) ((readBuf[11] << 8) | readBuf[12]));
    parameter(PUSH, PAN_VELOCITY_P, (INT32S) ((readBuf[13] << 8) | readBuf[14]));
    parameter(PUSH, TILT_VELOCITY_P, (INT32S) ((readBuf[15] << 8) | readBuf[16]));

    /* TODO

	  parameter(PUSH, FREE_P, (INT32S) readBuf[]);
	  */

    wd_kick_from_task(); /* Kick the dog to let it know the task is running */
    vTaskDelay(PAR_UPDATER_TASK_FREQUENCY);
  }
}

BOOLEAN par_updater_init(void)
{
  if (xTaskCreate(param_updater_task, (signed portCHAR *) "PAR_UPDATE", PAR_UPDATER_TASK_STACK_SIZE, NULL, PAR_UPDATER_TASK_PRIORITY, NULL) == pdPASS)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}




