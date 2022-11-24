#ifndef LORA_H
#define LORA_H
#include "spi.h"

#define CH20 911500000
#define FSTEP 61

#define RX_BUFFER_SIZE 256

extern u8 rxtxbuf[RX_BUFFER_SIZE];

void lora_init();
void set_channel(u32 frequency);
void set_mode(u8 mode);
void set_modemconfig2_rxpayloadcrcon(u8 crc_stat);
void set_modemconfig2_sf();
void idle_mode(void);
void Tx();
void Rx();
void write_fifo(u8* buffer, u8 size);
void read_fifo(u8* buffer, u8 size);
void Send(u8* buffer, u8 size);
void RxTimeout_interrupt();
void RxTxDone_interrupt();

#endif // !1