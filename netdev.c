#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <util/delay.h>
#include "enc28j60.h"
#include "spi.h"

#define BAUD 9600
#define UBRR_VAL F_CPU/16/BAUD-1

static int uart_putchar(char c, FILE * file);
char uart_getchar(void);
static FILE uart = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

int main()
{
	UBRR0H = (UBRR_VAL) >> 8;
	UBRR0L = UBRR_VAL;

	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	stdout = &uart;

	spi_initialize();
	
	uint8_t mac[2][6] = {
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, //Destination
	{0x01, 0x02, 0x03, 0x04, 0x05, 0x06}}; //Source

	int size = 64;//2 * 6 + 30;
	uint8_t * buf = calloc(size, sizeof(uint8_t));

	memcpy(buf, mac, 12);
	memcpy(buf + 12, "\x08\x06", 2);
	memcpy(buf + 14, "\x00\x01", 2);
	memcpy(buf + 16, "\x08\x00", 2);
	memcpy(buf + 18, "\x06\x04", 2);
	memcpy(buf + 20, "\x00\x02", 2);
	memcpy(buf + 22, mac[1], 6);
	memcpy(buf + 28, "\xA9\xFE\x09\xC0", 4);
	memcpy(buf + 32, mac[0], 6);
	memcpy(buf + 38, "\x00\x00\x00\x00", 4);
	//memcpy(buf + 32, "\x00\x26\x18\x7E\xF5\x9F", 6);
	//memcpy(buf + 38, "\xA9\xFE\x09\xC0", 4);
	memcpy(buf + 60, "\xfe\xca\xa1\xfc", 4);


	ethernet_initialize(mac[1]);

	puts("Going into loop");

	while(1) {
		printf("Number of packets available: %d\n", packets_available());
		_delay_ms(10000);
		puts("Sending packet ARP");
		transmit_data(buf, size);
	}

	free(buf);
}

static int uart_putchar(char c, FILE * file)
{
    if (c == '\n') {
		uart_putchar('\r', file);
	}

    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;

    return 0;
}

char uart_getchar(void)
{
    while(!(UCSR0A & (1<<RXC0)));
    return(UDR0);
}
