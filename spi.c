#include "spi.h"
#include <avr/io.h>
#include <stddef.h>

void spi_initialize()
{
	DDRB = (1<<CS) | (1<<MOSI) | (0<<MISO) | (1<<SCK);
	PORTB |= (1<<CS);
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
}

uint8_t spi_write(uint8_t * data, size_t size)
{
	PORTB &= ~(1<<CS);
	
	for(size_t i = 0; i < size; i++) {
		SPDR = data[i];
		while(!(SPSR & (1<<SPIF)));
	}

	PORTB |= (1<<CS);
	return SPDR;
}
