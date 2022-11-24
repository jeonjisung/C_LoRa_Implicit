#include "lora.h"

u8 rxtxbuf[RX_BUFFER_SIZE];

void lora_init()
{
    //주파수 설정, 다음 레지스터를 사용하기 위해 설정
    set_channel(CH20);

    // LongRangeMode로 전환하기 위한 SLEEP 모드 설정
    set_mode(RFLR_OPMODE_SLEEP);
    // FSK/OK MODE, LoRa 모드로 전환하기 위해 LongRangeMode 설정
    write_reg_single(REG_LR_OPMODE, (read_reg_single(REG_LR_OPMODE) & RFLR_OPMODE_LONGRANGEMODE_MASK) | RFLR_OPMODE_LONGRANGEMODE_ON);
    write_reg_single(REG_LR_OPMODE, (read_reg_single(REG_LR_OPMODE) & RFLR_OPMODE_FREQMODE_ACCESS_MASK) | RFLR_OPMODE_FREQMODE_ACCESS_HF);

    // DIO 0 에서 DIO 3으로 매핑, 기본값을 가지는 이유 : DIO 0~3의 기능들을 사용하기 위해.
    write_reg_single(REG_LR_DIOMAPPING1, 0x00);
    // Implicit Header Mode, 전송시간 줄이는데 사용
    set_modemconfig2_rxpayloadcrcon(1);
    // SF(확산계수) 설정
    set_modemconfig2_sf();
}

void set_channel(u32 frequency)
{
    u32 frf = frequency / FSTEP;
    // MSB(Most Significant Bit)
    write_reg_single(REG_LR_FRFMSB, (frf & 0xFFFFFF) >> 16);
    //중간 자리 비트
    write_reg_single(REG_LR_FRFMID, (frf & 0xFFFF) >> 8);
    // LSB(Least Significant Bit)
    write_reg_single(REG_LR_FRFLSB, (frf & 0xFF));
}

void set_mode(u8 mode)
{
    write_reg_single(REG_LR_OPMODE, (read_reg_single(REG_LR_OPMODE) & RFLR_OPMODE_MASK) | mode);
}

void set_modemconfig2_rxpayloadcrcon(u8 crc)
{
    // Implicit Header Mode로 설정하려면 crc가 1이 되어야함.
    if (crc)
        write_reg_single(REG_LR_MODEMCONFIG2, (read_reg_single(REG_LR_MODEMCONFIG2) & RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK) | RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON);
    //그렇지 않으면 Implicit Header Mode는 꺼진다.
    else
        write_reg_single(REG_LR_MODEMCONFIG2, (read_reg_single(REG_LR_MODEMCONFIG2) & RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK) | RFLR_MODEMCONFIG2_RXPAYLOADCRC_OFF);
}

void set_modemconfig2_sf()
{
    write_reg_single(REG_LR_MODEMCONFIG2, (read_reg_single(REG_LR_MODEMCONFIG2) & RFLR_MODEMCONFIG2_SF_MASK) | RFLR_MODEMCONFIG2_SF_7);
}

//전송 정보가 저장되는 메모리의 지점을 지정
void tx_base_addr(u8 addr)
{
    write_reg_single(REG_LR_FIFOTXBASEADDR, addr);
}
//정보가 있는 데이터 버퍼의 지점을 나타냄.
void rx_base_addr(u8 addr)
{
    write_reg_single(REG_LR_FIFORXBASEADDR, addr);
}

void idle_mode(void)
{
    write_reg_single(REG_LR_OPMODE, (read_reg_single(REG_LR_OPMODE) & RFLR_OPMODE_MASK) | RFLR_OPMODE_STANDBY);
}

void write_fifo(u8 *buffer, u8 size)
{
    write_reg_multi(0, buffer, size);
}

void read_fifo(u8 *buffer, u8 size)
{
    read_reg_multi(0, buffer, size);
}

void Tx()
{
    write_reg_single(REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
                                              RFLR_IRQFLAGS_RXDONE |
                                              RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                              RFLR_IRQFLAGS_VALIDHEADER |
                                              RFLR_IRQFLAGS_CADDONE |
                                              RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                              RFLR_IRQFLAGS_CADDETECTED);

    write_reg_single(REG_LR_DIOMAPPING1, (read_reg_single(REG_LR_DIOMAPPING1) & RFLR_DIOMAPPING1_DIO0_MASK) | RFLR_DIOMAPPING1_DIO0_01);

    set_mode(RFLR_OPMODE_TRANSMITTER);
}

void Rx()
{
    write_reg_single(REG_LR_INVERTIQ, ((read_reg_single(REG_LR_INVERTIQ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK) | RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_OFF));
    write_reg_single(REG_LR_INVERTIQ2, RFLR_INVERTIQ2_OFF);
    write_reg_single(REG_LR_IRQFLAGSMASK,
                     RFLR_IRQFLAGS_VALIDHEADER |
                         RFLR_IRQFLAGS_TXDONE |
                         RFLR_IRQFLAGS_CADDONE |
                         RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                         RFLR_IRQFLAGS_CADDETECTED);

    write_reg_single(REG_LR_DIOMAPPING1, (read_reg_single(REG_LR_DIOMAPPING1) & RFLR_DIOMAPPING1_DIO0_MASK) | RFLR_DIOMAPPING1_DIO0_00);
    write_reg_single(REG_LR_DIOMAPPING1, (read_reg_single(REG_LR_DIOMAPPING1) & RFLR_DIOMAPPING1_DIO1_MASK) | RFLR_DIOMAPPING1_DIO1_00);

    write_reg_single(REG_LR_FIFORXBASEADDR, 0);
    write_reg_single(REG_LR_FIFOADDRPTR, 0);

    memset(rxtxbuf, 0, RX_BUFFER_SIZE);

    set_mode(RFLR_OPMODE_RECEIVER_SINGLE);
}

void Send(u8 *buffer, u8 size)
{
    // Payload 크기 초기화
    write_reg_single(REG_LR_PAYLOADLENGTH, size);

    // Tx에 사용되는 전체 버퍼
    write_reg_single(REG_LR_FIFOTXBASEADDR, 0);
    write_reg_single(REG_LR_FIFOADDRPTR, 0);

    // Sleep 모드인지 확인, Sleep 모드에서는 FIFO작업 수행x
    if ((read_reg_single(REG_LR_OPMODE) & ~RFLR_OPMODE_MASK) == RFLR_OPMODE_SLEEP)
    {
        idle_mode();
        wait_ms(1);
    }
    // write_reg_single payload buffer
    write_fifo(buffer, size);
    Tx();
}