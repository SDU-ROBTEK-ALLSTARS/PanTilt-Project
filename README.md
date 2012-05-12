# Pan-Tilt System #
The system is split in two main parts, which are coorporating: ARM software on a LM3S6965 (Cortex-M3) and FPGA curcuits targetting the Xilinx Spartan-E3.

The FPGA runs dc motors on the pan/tilt frame and reads back encoder input to communicate to the microcontroller, which is the *controller* that manipulates input values to the FPGA+motor system.

## Directory structure ##
An overview of the directory structure.

    ./
    +-arm/                C code for ARM Cortex-M3 target
	| +-src/
	| | +-empcourse/      EMP-course definitions
    | | +-freertos/       FreeRTOS source
    | | +-modules/        Each code module lives here
	| | +-test/           Unit-test applications
	| +-lib/
	|   +-StellarisWare/  StellarisWare drivers
    +-doc/
    +-fpga/               VHDL designs for Spartan-XX target
	+-tools/              Utilities


## ARM software ##
This part of the README discusses the C code/microcontroller part of the project.

### Compiling the code ###
A Makefile is supplied for GNU make to build the program. It requires a "bare-metal" ARM toolchain (arm-none-eabi-) to be available on the $PATH.

For those using Red Suite (Eclipse) the Makefile can still be used to build the project. Create a new project with "New" > "Other" > "C/C++" > "Makefile project with existing code" and then make sure an appropiate toolchain (Code Red tools?) is chosen in the project options under "Toolchain Editor".

The application has, thus far, only been tested with the Newlib C standard library implementation.

