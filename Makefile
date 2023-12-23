CC=avr-gcc
OBJCOPY=avr-objcopy
ProjectName=smartDistance

CFLAGS=-Os -DF_CPU=16000000UL -mmcu=atmega328p -Wall
PORT=/dev/ttyACM0

$(ProjectName).hex: $(ProjectName).o
	$(OBJCOPY) -O ihex -R .eeprom $(ProjectName).o $(ProjectName).hex

$(ProjectName).o:
	$(CC) $(CFLAGS) -o $(ProjectName).o main.c

install: $(ProjectName).hex
	avrdude -F -V -c arduino -p ATMEGA328P -P $(PORT) -b 115200 -U flash:w:$(ProjectName).hex

clean:
	rm -f $(ProjectName).o $(ProjectName).hex