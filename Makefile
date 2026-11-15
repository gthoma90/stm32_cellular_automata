TARGET = i2c_display
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CMSIS = ../STM32CubeF1/Drivers/CMSIS

CFLAGS = -mcpu=cortex-m3 -mthumb -nostdlib -nostartfiles -O0 -g
CFLAGS += -DSTM32F103x6
CFLAGS += -I$(CMSIS)/Include
CFLAGS += -I$(CMSIS)/Device/ST/STM32F1xx/Include

SRCS = startup.c main.c ssd1306.c automata.c graphics.c i2c1.c encoder.c systick.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET).bin $(TARGET).hex

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) -T stm32f103.ld $(OBJS) -o $(TARGET).elf

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex --set-start 0x08000000 $(TARGET).elf $(TARGET).hex

flash: $(TARGET).bin
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $(TARGET).bin 0x08000000 verify reset exit"

flash-hex: $(TARGET).hex
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $(TARGET).hex verify reset exit"

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).bin $(TARGET).hex
