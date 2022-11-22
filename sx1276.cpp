#include "reg.h"
#include "sx1276.h"

u8 rxtxbuf[RX_BUFFER_SIZE];

void lora_init(){

}

void lora_frequency(u32 frequency){
    u32 frf = frequency / FSTEP;
    write_reg_single(REG_LR_FRFMSB, (frf & 0xFFFFFF) >> 16);
    write_reg_single(REG_LR_FRFMID, (frf & 0xFFFF) >> 8);
    write_reg_single(REG_LR_FRFLSB, (frf & 0xFF));
}