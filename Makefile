FLAGS = -Wall -Os -std=c99 -D__AVR_ATmega168__ -DF_CPU=10000000UL
CC = avr-gcc

all: netdev
	avr-objcopy -O ihex netdev.hex netdev.hex
	avrdude -c dragon_isp -p m168 -P usb -U flash:w:netdev.hex -v

netdev: enc28j60.o spi.o netdev.o
	$(CC) $(FLAGS) -o netdev.hex enc28j60.o spi.o netdev.o

enc28j60.o: enc28j60.c enc28j60.h
	$(CC) $(FLAGS) -c enc28j60.c

spi.o: spi.c spi.h
	$(CC) $(FLAGS) -c spi.c

netdev.o: netdev.c
	$(CC) $(FLAGS) -c netdev.c

clean:
	rm *.o netdev.hex
