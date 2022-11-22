#ifndef SX1276_H
#define SX1276_H
#include "spi.h"

#define CH20 921500000
#define FSTEP 61

#define TIMEOUT 0xBB8
#define RX_BUFFER_SIZE 256

extern u8 rxtxbuf[RX_BUFFER_SIZE];

void lora_init();
void lora_frequency(u32 frequency);

#endif // !1