
# Project options
PROJ_NAME = pantilt
BUILD_TOP_DIR = Build

# GNU tools
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# Linker script (memory placement)
LDSCRIPT = standalone.ld

# "Top level" directories
SRC_DIR = src
LIB_DIR = lib
RTOS_SOURCE_DIR = $(SRC_DIR)/freertos/Source
MODULES_DIR = $(SRC_DIR)/modules
TEST_DIR = $(SRC_DIR)/test
GLOBAL_DIR = $(SRC_DIR)/global
STELLARISWARE_DIR = $(LIB_DIR)/StellarisWare

# Directories that the compiler should look for #includes in
INCLUDE_DIRS =\
	-I . \
	-I $(SRC_DIR) \
	-I $(GLOBAL_DIR) \
	-I $(MODULES_DIR) \
	-I $(STELLARISWARE_DIR) \
	-I $(RTOS_SOURCE_DIR)/include \
	-I $(RTOS_SOURCE_DIR)/portable/GCC/ARM_CM3

# Source files to be compiled
SOURCE =\
	$(SRC_DIR)/startup.c \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/syscalls.c \
	$(MODULES_DIR)/comm/spi.c \
	$(MODULES_DIR)/comm/uart.c \
	$(MODULES_DIR)/comm/uart_protocol.c \
	$(MODULES_DIR)/comm/uart_echo.c \
	$(MODULES_DIR)/comm/uart_to_spi.c \
	$(MODULES_DIR)/debug/debug.c \
	$(MODULES_DIR)/adc/adc.c \
	$(MODULES_DIR)/control/control.c \
	$(MODULES_DIR)/dreh/dreh.c \
	$(MODULES_DIR)/itc/itc.c \
	$(MODULES_DIR)/lcd16x2/lcd.c \
	$(MODULES_DIR)/lcd16x2/lcd_port.c \
	$(MODULES_DIR)/leds/leds.c \
	$(MODULES_DIR)/leds/status_led.c \
	$(MODULES_DIR)/menu/menu.c \
	$(MODULES_DIR)/menu/menu_setup.c \
	$(MODULES_DIR)/numpad/numpad.c \
	$(TEST_DIR)/comm/spi_test.c \
	$(TEST_DIR)/sys/runtimestats.c \
	$(RTOS_SOURCE_DIR)/list.c \
	$(RTOS_SOURCE_DIR)/queue.c \
	$(RTOS_SOURCE_DIR)/tasks.c \
	$(RTOS_SOURCE_DIR)/timers.c \
	$(RTOS_SOURCE_DIR)/portable/GCC/ARM_CM3/port.c \
	$(RTOS_SOURCE_DIR)/portable/MemMang/heap_2.c

# Compiler flags
CFLAGS =\
	-DGCC_ARMCM3 \
	-D__NEWLIB__ \
	-std=c99 \
	-mthumb \
	-mcpu=cortex-m3 \
	-Wall \
	-ffunction-sections \
	-fdata-sections \
	$(INCLUDE_DIRS)

# Linker flags
LINKER_FLAGS =\
	-nostartfiles \
	-Xlinker -o$(BUILD_DIR)/$(PROJ_NAME).axf \
	-Xlinker -M \
	-Xlinker -Map=$(BUILD_DIR)/$(PROJ_NAME).map \
	-T$(LDSCRIPT) \
	-Xlinker --gc-sections

LIBS =\
	$(STELLARISWARE_DIR)/driverlib/gcc-cm3/libdriver-cm3.a

BUILD_DIR = $(BUILD_TOP_DIR)

#OBJS = $(patsubst %.c,$(BUILD_DIR)/obj/%.o,$(SOURCE))
OBJS = $(SOURCE:.c=.o)

# Targets
all: debug

debug: CFLAGS += -O0 -g -DDEBUG
debug: binary

release: CFLAGS += -s -O2
release: binary

binary: $(BUILD_DIR)
binary: $(BUILD_DIR)/$(PROJ_NAME).bin
binary: postbuild

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

#@mkdir -p $(BUILD_DIR)/obj

$(BUILD_DIR)/$(PROJ_NAME).bin: $(BUILD_DIR)/$(PROJ_NAME).axf
	@echo "OBJCOPY\t$^ -> $@"
	@$(OBJCOPY) $(BUILD_DIR)/$(PROJ_NAME).axf -O binary $(BUILD_DIR)/$(PROJ_NAME).bin

$(BUILD_DIR)/$(PROJ_NAME).axf: $(OBJS)
	@echo "LD\t$(^F) -> $@"
	@$(CC) $(CFLAGS) $(OBJS) $(LIBS) $(LINKER_FLAGS)

$(OBJS): %.o : %.c
	@echo "CC\t$< -> $@"
	@$(CC) $(CFLAGS) -c $< -o $@

#@mkdir -p $(@D)

#$(LIBS):
#	@make -C $(STELLARISWARE_DIR)/driverlib -f Makefile.driverlib-cm3

postbuild:
	@echo "SIZE\t$(BUILD_DIR)/$(PROJ_NAME).axf"
	@$(SIZE) $(BUILD_DIR)/$(PROJ_NAME).axf
	@echo "SIZE\t$(BUILD_DIR)/$(PROJ_NAME).bin"
	@$(SIZE) --target=binary $(BUILD_DIR)/$(PROJ_NAME).bin

cleandebug: clean

cleanrelease: clean

clean:
	@touch Makefile
	@rm -v -R $(BUILD_TOP_DIR)
	@rm -v $(OBJS)

#cleanlibs:
#	@make clean -C $(STELLARISWARE_DIR)/driverlib -f Makefile.driverlib-cm3

program:
	@openocd -d0 -f "board/ek-lm3s6965.cfg" -c "init" -c "reset init" -c "halt" -c "flash write_image erase $(BUILD_DIR)/$(PROJ_NAME).bin" -c "verify_image $(BUILD_DIR)/$(PROJ_NAME).bin" -c "reset" -c shutdown

