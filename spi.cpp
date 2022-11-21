#include "mbed.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define MOSI PB_5
#define MISO PB_4
#define SCK PB_3
#define NSS PA_15

#define LOW 0
#define HIGH 1

#define NSS_HIGH cs = HIGH;
#define NSS_LOW cs = LOW;

#define _delay wait_us(100);

void write_reg_single(uint8_t reg, uint8_t data);
void write_reg_multi(uint8_t reg, uint8_t* data, uint8_t size);
uint8_t read_reg_single(uint8_t reg);
void read_reg_multi(uint8_t reg, uint8_t* buffer, uint8_t size);
void spi_init();

SPI spi(MOSI, MISO, SCK);
DigitalOut cs(NSS); 

void spi_init(){
	NSS_HIGH;

	spi.format(8, 3);
	spi.frequency(10000000);
	_delay;
}

void write_reg_single(uint8_t reg, uint8_t data){
	write_reg_multi(reg, &data, 1);
}

void write_reg_multi(uint8_t reg, uint8_t* data, uint8_t size){
	NSS_LOW;

	spi.write(reg | 0x80);

	for(int i = 0; i < size; ++i){
		spi.write(data[i]);
	}
	NSS_HIGH;
}

uint8_t read_reg_single(uint8_t reg){
	uint8_t byte = 0;
	read_reg_multi(reg, &byte, 1);
	return byte;
}

void read_reg_multi(uint8_t reg, uint8_t* buffer, uint8_t size){
	NSS_LOW;

	spi.write(reg & 0x7F);
	
	for(int i = 0; i < size; ++i){
		buffer[i] = spi.write(0);
	}

	NSS_HIGH;
}