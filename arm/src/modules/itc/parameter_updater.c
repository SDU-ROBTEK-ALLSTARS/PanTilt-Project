
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
                         ADDRESS_VEL_B_MSB, ADDRESS_VEL_B_LSB
                         /*(ADDRESS_AUX_FROM_ARM | 0x80), ADDRESS_AUX_TO_ARM,
                         0x00*/};
  INT8U readBuf[sizeof(transmitBuf)];
  INT32S temp;

  while(1)
  {
    temp = parameter(POP, PAN_PWM_P);
    transmitBuf[1] = (INT8U) (temp >> 8);
    transmitBuf[3] = (INT8U) temp;

    temp = parameter(POP, TILT_PWM_P);
    transmitBuf[5] = (INT8U) (temp >> 8);
    transmitBuf[7] = (INT8U) temp;

    for (INT32U i=0; i<sizeof(transmitBuf); i++)
    {
      spi_write_from_task(&transmitBuf[i], 1, portMAX_DELAY);
      spi_read_from_task(&readBuf[i], 1, portMAX_DELAY);
    }

    parameter(PUSH, TILT_POSITION_P, (INT32S) ((readBuf[8] << 8) + readBuf[9]));
    parameter(PUSH, PAN_POSITION_P, (INT32S) ((readBuf[10] << 8) + readBuf[11]));
//    parameter(PUSH, TILT_POSITION_P, (INT32S) ((0xA3 << 8) + 0x28));
//    parameter(PUSH, PAN_POSITION_P, (INT32S) ((0x27 << 8) + 0x10));


    parameter(PUSH, PAN_VELOCITY_P, (INT32S) ((readBuf[12] << 8) + readBuf[13]));
    parameter(PUSH, TILT_VELOCITY_P, (INT32S) ((readBuf[14] << 8) + readBuf[15]));
    //parameter(PUSH, FREE_P, (INT32S) readBuf[18]);

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




