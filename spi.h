#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <stddef.h>

#define CS 2
#define MOSI 3
#define MISO 4
#define SCK 5

void spi_initialize();
uint8_t spi_write(uint8_t * data, size_t size);

#endif
