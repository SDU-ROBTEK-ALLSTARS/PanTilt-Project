
# Project options
PROJ_NAME = pantilt
BUILD_TOP_DIR = Build

# GNU tools
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# Linker script (memory placement)
LDSCRIPT = lm3s6569.ld

# "Top level" directories
RTOS_SOURCE_DIR = freertos/Source
MODULES_DIR = modules

# Directories that the compiler should look for #includes in
INCLUDE_DIRS =\
	-I . \
	-I $(MODULES_DIR) \
	-I $(RTOS_SOURCE_DIR)/include \
	-I $(RTOS_SOURCE_DIR)/portable/GCC/ARM_CM3

# Source files to be compiled
SOURCE =\
	startup.c \
	main.c \
	$(MODULES_DIR)/comm/spi.c \
	$(MODULES_DIR)/debug/debug.c \
	$(MODULES_DIR)/sysctl/sysctl.c \
	$(RTOS_SOURCE_DIR)/list.c \
	$(RTOS_SOURCE_DIR)/queue.c \
	$(RTOS_SOURCE_DIR)/tasks.c \
	$(RTOS_SOURCE_DIR)/portable/GCC/ARM_CM3/port.c \
	$(RTOS_SOURCE_DIR)/portable/MemMang/heap_2.c

# Compiler flags
CFLAGS =\
	-DGCC_ARMCM3 \
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

#LIBS= $(LUMINARY_DRIVER_DIR)/arm-none-eabi-gcc/libdriver.a $(LUMINARY_DRIVER_DIR)/arm-none-eabi-gcc/libgr.a

OBJS = $(patsubst %.c,$(BUILD_DIR)/obj/%.o,$(SOURCE))

BUILD_DIR = $(BUILD_TOP_DIR)

# Targets
all: debug

debug: CFLAGS += -g -DDEBUG
debug: binary

release: CFLAGS += -s -O2
release: binary

binary: $(BUILD_DIR)
binary: $(BUILD_DIR)/$(PROJ_NAME).bin
binary: postbuild

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/obj

$(BUILD_DIR)/$(PROJ_NAME).bin: $(BUILD_DIR)/$(PROJ_NAME).axf
	@echo '# Creating binary: $@...'
	$(OBJCOPY) $(BUILD_DIR)/$(PROJ_NAME).axf -O binary $(BUILD_DIR)/$(PROJ_NAME).bin
	@echo ' '

$(BUILD_DIR)/$(PROJ_NAME).axf: $(OBJS)
	@echo '# Linking to: $@...'
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) $(LINKER_FLAGS)
	@echo ' '

$(OBJS): $(BUILD_DIR)/obj/%.o : %.c
	@echo '# Compiling: $<...'
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo ' '

postbuild:
	$(SIZE) $(BUILD_DIR)/$(PROJ_NAME).axf

cleandebug: clean

cleanrelease: clean

clean:
	@touch Makefile
	@rm -v -R $(BUILD_TOP_DIR)

program:
	openocd -f "board/ek-lm3s6965.cfg" -c "init" -c "reset init" -c "halt" -c "flash write_image erase $(BUILD_DIR)/$(PROJ_NAME).bin" -c "verify_image $(BUILD_DIR)/$(PROJ_NAME).bin" -c "reset" -c shutdown

