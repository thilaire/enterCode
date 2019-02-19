# Compilation options
CC = avr-gcc
OBJCPY = avr-objcopy
CCFLAGS = -Wall -O3
#OPTIM =
OPTIM = -Os

# about the AVR
DEVICE = attiny2313
FREQ = 8000000UL


# files
OUT = enterCode
SRC = enterCode.c TM1637.c
OBJ = $(SRC:.c=.o)

all: $(OUT).hex

# Compile the C files
%.o: %.c
	$(CC) $(CCFLAGS) $(OPTIM) -mmcu=$(DEVICE) -DF_CPU=$(FREQ) -o $@ -c $<

# Link them
$(OUT).elf: $(OBJ)
	$(CC) -mmcu=$(DEVICE) -o $@ $^

# Generate the hex
$(OUT).hex: $(OUT).elf
	$(OBJCPY) -j .text -j .data -O ihex $< $@

assembly: $(SRC:.c=.s)
%.s : %.c %.o
	$(CC) $(CCFLAGS) $(OPTIM) -mmcu=$(DEVICE) -DF_CPU=$(FREQ) -S -fverbose-asm $< -o $@

# Flash the AVR (with avrdude)
# HACK: first get the eeprom.hex (read the eeprom) and then re-program this eeprom (and delete the eeprom.hex)
program: $(OUT).hex eeprom.hex
	avrdude -c usbasp -p $(DEVICE) -U flash:w:$< -U eeprom:w:eeprom.hex:i
	rm eeprom.hex

eeprom.hex:
	avrdude -c usbasp -p $(DEVICE) -U eeprom:r:eeprom.hex:i

# program the fuses
fuses:
	avrdude -c usbasp -p $(DEVICE) -U lfuse:w:0xE4:m 	-U hfuse:w:0xDF:m

# program the eeprom
eeprom:
	avrdude -c usbasp -p $(DEVICE) -U eeprom:w:eepromInit.hex

# Clean the mess
clean:
	rm -f $(OBJ) $(OUT).elf $(OUT).hex $(SRC:.c=.s)
	rm -f $(OUT).hex

