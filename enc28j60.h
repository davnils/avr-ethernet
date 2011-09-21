#ifndef ENC28J60_H
#define ENC28J60_H

#define READ_CONTROL_REGISTER 0b000
#define READ_BUFFER_MEMORY 0b001
#define WRITE_CONTROL_REGISTER 0b010
#define WRITE_BUFFER_MEMORY 0b011
#define BIT_FIELD_SET 0b100
#define BIT_FIELD_CLEAR 0b101
#define SYSTEM_RESET_COMMAND 0b111

//Shared over all banks
#define EIE 0x1B
#define EIR 0x1C
#define ESTAT 0x1D
#define ECON2 0x1E
#define ECON1 0x1F

#define CLKRDY 0
#define MARXEN 0
#define AUTOINC 7
#define TXRTS 3
#define TXABRT 1
#define RXEN 2
#define LATECOL 4

//Bank 0
#define ERDPTL 0x00
#define ERDPTH 0x01
#define EWRPTL 0x02
#define EWRPTH 0x03
#define ETXSTL 0x04
#define ETXSTH 0x05
#define ETXNDL 0x06
#define ETXNDH 0x07
#define ERXSTL 0x08
#define ERXSTH 0x09
#define ERXNDL 0x0A
#define ERXNDH 0x0B
#define ERXRDPTL 0x0C
#define ERXRDPTH 0x0D
#define ERXWRPTL 0x0E
#define ERXWRPTH 0x0F
//...

//Bank 1
//...
#define EPKTCNT 0x19

//Bank 2
#define MACON1 0x00
#define MACON3 0x02
#define MACON4 0x03
#define MABBIPG 0x04
#define MAIPGL 0x06
#define MAIPGH 0x07
//...
#define MAMXFLL 0x0A
#define MAMXFLH 0x0B
//...
#define MIREGADR 0x14
#define MIWRL 0x16
#define MIWRH 0x17

//Bank 3
#define MAADR5 0x00
#define MAADR6 0x01
#define MAADR3 0x02
#define MAADR4 0x03
#define MAADR1 0x04
#define MAADR2 0x05
//...
#define MISTAT 0x0A
//...

#define BUSY 0

//Phy registers
#define PHCON1 0x00
#define PHCON2 0x10
#define PHLCON 0x14

#define HDLDIS 8 //TODO: control usage of 16/8 bits 

uint8_t spi_transaction(uint8_t opcode, uint8_t arg, uint8_t data);
/*void test();
void test_read();*/
void set_mac(uint8_t mac[6]);
void write_phy(uint8_t reg, uint16_t data);
uint16_t read_phy(uint8_t reg);
void ethernet_initialize(uint8_t mac[6]);
int packets_available();
void transmit_data(uint8_t * data, size_t len);


#endif
