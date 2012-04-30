# Pan-Tilt System #
The system is split in two main parts, which are coorporating: ARM software and FPGA curcuit design.

## Directory structure ##
Feel free and add/edit this as the project develops

    ./
    +-arm/               C code for ARM Cortex-M3 target
	| +-src/
	| | +-empcourse/     EMP-course definitions
    | | +-freertos/      FreeRTOS source
    | | +-modules/       Each code module lives here
	| | +-test/          Unit-test applications
	| +-lib/
	|   +-SW-DRL-8555/   StellarisWare drivers
	|
    +-doc/
	|
    +-fpga/              VHDL designs for Spartan-XX target


## ARM software ##
This part of the README discusses the C code/microcontroller part of the project.

### Compiling the code ###
A Makefile is supplied for GNU make to build the program. It requires a GNU "bare-metal" ARM toolchain (arm-none-eabi-) to be available in the $PATH.

For those using Red Suite (Eclipse) the Makefile can still be used to build the project. Create a new project with "New" > "Other" > "C/C++" > "Makefile project with existing code" and then make sure an appropiate toolchain (Code Red GNU tools?) is chosen in the project options under "Toolchain Editor".

The application is, thus far, only tested with the Newlib C standard library implementation.

### C code guidelines ###


## FPGA design ##
