TARGET = i2c_display
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# External STM32CubeF1 checkout supplies the CMSIS core and device headers.
CMSIS = ../STM32CubeF1/Drivers/CMSIS

.PHONY: all clean flash flash-hex

CFLAGS = -mcpu=cortex-m3 -mthumb -nostdlib -nostartfiles -O0 -g
CFLAGS += -Wall -Wextra
CFLAGS += -DSTM32F103x6
CFLAGS += -Iinclude
CFLAGS += -I$(CMSIS)/Include
CFLAGS += -I$(CMSIS)/Device/ST/STM32F1xx/Include
# Generate header dependencies and tolerate dependencies on removed headers.
CFLAGS += -MMD -MP

SRCS = startup/startup.c src/main.c src/ssd1306.c src/automata.c src/graphics.c src/i2c1.c src/encoder.c src/systick.c
OBJS = $(SRCS:.c=.o)
LDSCRIPT = startup/stm32f103.ld

all: $(TARGET).bin $(TARGET).hex

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS) $(LDSCRIPT)
	$(CC) $(CFLAGS) -T $(LDSCRIPT) $(OBJS) -o $(TARGET).elf

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex --set-start 0x08000000 $(TARGET).elf $(TARGET).hex

flash: $(TARGET).bin
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $(TARGET).bin 0x08000000 verify reset exit"

flash-hex: $(TARGET).hex
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $(TARGET).hex verify reset exit"

-include $(OBJS:.o=.d)

clean:
	rm -f $(OBJS) $(OBJS:.o=.d) $(TARGET).elf $(TARGET).bin $(TARGET).hex
