#include "spi.h"
#include "enc28j60.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>

static void switch_bank(int bank)
{
	spi_transaction(BIT_FIELD_CLEAR, ECON1, 0x3);
	spi_transaction(BIT_FIELD_SET, ECON1, bank & 0x3);
}

void set_mac(uint8_t mac[6])
{
	switch_bank(3);
	spi_transaction(WRITE_CONTROL_REGISTER, MAADR1, mac[0]);
	spi_transaction(WRITE_CONTROL_REGISTER, MAADR2, mac[1]);
	spi_transaction(WRITE_CONTROL_REGISTER, MAADR3, mac[2]);
	spi_transaction(WRITE_CONTROL_REGISTER, MAADR4, mac[3]);
	spi_transaction(WRITE_CONTROL_REGISTER, MAADR5, mac[4]);
	spi_transaction(WRITE_CONTROL_REGISTER, MAADR6, mac[5]);
}

void write_phy(uint8_t reg, uint16_t data)
{
	switch_bank(2);
	spi_transaction(WRITE_CONTROL_REGISTER, MIREGADR, reg);
	spi_transaction(WRITE_CONTROL_REGISTER, MIWRL, data & 0xFF);
	spi_transaction(WRITE_CONTROL_REGISTER, MIWRH, (data>>8) & 0xFF);

	switch_bank(3);	

	uint8_t read;
	do {
		read = spi_transaction(READ_CONTROL_REGISTER, MISTAT, 0);
	} while(read & (1<<BUSY));
}

//TODO:
uint16_t read_phy(uint8_t reg)
{
	return 0x0000;
}

void ethernet_initialize(uint8_t mac[6])
{
	//Conf the LEDs
	write_phy(PHLCON, 0b0011110000010011);

	//Configure some buffers and MAC stuff for half duplex
	//Recieve buffer spans 0x0000 to 0x0FFF
	switch_bank(0);
	spi_transaction(WRITE_CONTROL_REGISTER, ERXSTL, 0x00);
	spi_transaction(WRITE_CONTROL_REGISTER, ERXSTH, 0x00);
	spi_transaction(WRITE_CONTROL_REGISTER, ERXNDL, 0xFF);
	spi_transaction(WRITE_CONTROL_REGISTER, ERXNDH, 0x0F);

	spi_transaction(WRITE_CONTROL_REGISTER, ERXRDPTL, 0x00);
	spi_transaction(WRITE_CONTROL_REGISTER, ERXRDPTH, 0x00);

	//Transmit buffers covers 0x1000 to 0x1FFF
	//...

	uint8_t reg;
	do {
		reg = spi_transaction(READ_CONTROL_REGISTER, ESTAT, 0);
	} while(!(reg & (1<<CLKRDY)));

	//MAC settings
	switch_bank(2);
	spi_transaction(WRITE_CONTROL_REGISTER, MACON1, (1<<MARXEN));
	spi_transaction(WRITE_CONTROL_REGISTER, MACON3, 0b11110000);
	spi_transaction(WRITE_CONTROL_REGISTER, MACON4, 0b01000000); //DEFER

	//1518 bytes	
	spi_transaction(WRITE_CONTROL_REGISTER, MAMXFLL, 0xEE);
	spi_transaction(WRITE_CONTROL_REGISTER, MAMXFLH, 0x05);

	spi_transaction(WRITE_CONTROL_REGISTER, MABBIPG, 0x12);
	spi_transaction(WRITE_CONTROL_REGISTER, MAIPGL, 0x12);
	spi_transaction(WRITE_CONTROL_REGISTER, MAIPGH, 0x0C);

	set_mac(mac);

	write_phy(PHCON2, (1<<HDLDIS));
	write_phy(PHCON1, 0);

	//Enable reception
	spi_transaction(BIT_FIELD_SET, ECON1, (1<<RXEN));
	puts("Enabled reception");
}

int packets_available()
{
	//Read EPKTCNT
	switch_bank(1);
	return(spi_transaction(READ_CONTROL_REGISTER, EPKTCNT, 0));
}

void receive_data(uint8_t * data, int max)
{
}

void transmit_data(uint8_t * data, size_t len)
{
	switch_bank(0);
	spi_transaction(WRITE_CONTROL_REGISTER, ETXSTL, 0x00);
	spi_transaction(WRITE_CONTROL_REGISTER, ETXSTH, 0x10);

	spi_transaction(WRITE_CONTROL_REGISTER, EWRPTL, 0x00);
	spi_transaction(WRITE_CONTROL_REGISTER, EWRPTH, 0x10);

	spi_transaction(WRITE_BUFFER_MEMORY, 0x1A, 0b00000000); //control byte

	for(size_t i = 0; i < len; i++) {
		spi_transaction(WRITE_BUFFER_MEMORY, 0x1A, data[i]);
	}

	uint16_t end = 0x1000 + len;
	spi_transaction(WRITE_CONTROL_REGISTER, ETXNDL, end & 0xFF);
	spi_transaction(WRITE_CONTROL_REGISTER, ETXNDH, (end>>8) & 0xFF);

	spi_transaction(BIT_FIELD_SET, ECON1, (1<<TXRTS));

	uint8_t reg;
	do {
		reg = spi_transaction(READ_CONTROL_REGISTER, ECON1, 0);
	} while(reg & (1<<TXRTS));

	reg = spi_transaction(READ_CONTROL_REGISTER, ESTAT, 0);
	if(reg & (1<<TXABRT)) {
		puts("Error occured during transmission");
		}
		if(reg & (1<<LATECOL)) {
			puts("Late collision occured");
		}
		spi_transaction(WRITE_CONTROL_REGISTER, ERDPTL, (end+1) & 0xFF);
		spi_transaction(WRITE_CONTROL_REGISTER, ERDPTH, ((end+1)>>8) & 0xFF);

		for(int i = 0; i < 7; i++) {
			printf("0x%02X ", spi_transaction(READ_BUFFER_MEMORY, 0x1A, 0));
		}
		puts("");
	//}
}

uint8_t spi_transaction(uint8_t opcode, uint8_t arg, uint8_t data)
{
	uint8_t send[2];
	send[0] = ((opcode & 0x7) << 5) | (arg & 0x1F);
	send[1] = data;
	return spi_write(send, 2);
}
