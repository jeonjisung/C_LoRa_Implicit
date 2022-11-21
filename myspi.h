#ifndef MYSPI_H
#define MYSPI_H

#include "mbed.h"
#include <stdio.h>
#include <string.h>
#include "sx1276resister.h"

#define MOSI PB_5
#define MISO PB_4
#define SCK PB_3
#define NSS PA_15

#define LOW 0
#define HIGH 1

#define U8 uint8_t
#define U16 uint16_t
#define U32 uint32_t

#define NSS_HIGH chip_sel = HIGH;
#define NSS_LOW chip_sel = LOW;

void write_reg_single(U8 reg, U8 data);
void write_reg_mul(U8 reg, U8* data, U8 size);
U8 read_reg_single(U8 reg);
void read_reg_mul(U8 reg, U8* buffer, U8 size);
void spiSetup();


#endif // !1