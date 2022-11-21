#include "myspi.h"

#define SPI_WRITE(x) device.write(x);

SPI device(MOSI, MISO, SCK);
DigitalOut chip_sel(NSS);

void spiSetup()
{
	NSS_HIGH;

	device.format(8, 0);
	device.frequency(10000000);

	wait_us(100);
}

void write_reg_single(U8 reg, U8 data)
{
	write_reg_mul(reg, &data, 1);
}

void write_reg_mul(U8 reg, U8* data, U8 size)
{
	NSS_LOW;
	SPI_WRITE(reg | 0x80);

	for (int i = 0; i < size; ++i)
	{
		SPI_WRITE(data[i]);
	}

	NSS_HIGH;
}

U8 read_reg_single(U8 reg)
{
	U8 byte = 0;
	read_reg_mul(reg, &byte, 1);
	return byte;
}

void read_reg_mul(U8 reg, U8* buffer, U8 size)
{
	NSS_LOW;
	SPI_WRITE(reg & 0x7F);

	for (int i = 0; i < size; ++i)
	{
		buffer[i] = SPI_WRITE(0);
	}

	NSS_HIGH;
}