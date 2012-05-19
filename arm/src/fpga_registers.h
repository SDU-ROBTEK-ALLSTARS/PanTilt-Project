
#ifndef FPGA_REGISTERS_H_
#define FPGA_REGISTERS_H_


/* FPGA register addresses */
#define ADDRESS_DUTY_A_MSB    0x02
#define ADDRESS_DUTY_A_LSB    0x03
#define ADDRESS_DUTY_B_MSB    0x04
#define ADDRESS_DUTY_B_LSB    0x05

#define ADDRESS_POS_A_MSB     0x06
#define ADDRESS_POS_A_LSB     0x07
#define ADDRESS_POS_B_MSB     0x08
#define ADDRESS_POS_B_LSB     0x09

#define ADDRESS_VEL_A_MSB     0x0A
#define ADDRESS_VEL_A_LSB     0x0B
#define ADDRESS_VEL_B_MSB     0x0C
#define ADDRESS_VEL_B_LSB     0x0D

#define ADDRESS_AUX_FROM_ARM  0x0E
#define ADDRESS_AUX_TO_ARM    0x0F


/* AUX register bit positions */
#define AUX_REG_WATCHDOG_BIT_0  4
#define AUX_REG_WATCHDOG_BIT_1  5


#endif /* FPGA_REGISTERS_H_ */
