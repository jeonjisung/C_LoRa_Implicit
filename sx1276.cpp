#include "sx1276.h"

U8 rxtxbuf[RX_BUFFER_SIZE];

void LoRasetup()
{
	set_frequency(CH20);

	set_operation_mode(RFLR_OPMODE_SLEEP);
	write_reg_single(REG_LR_OPMODE, (read_reg_single(REG_LR_OPMODE) & RFLR_OPMODE_LONGRANGEMODE_MASK) | RFLR_OPMODE_LONGRANGEMODE_ON);
	write_reg_single(REG_LR_OPMODE, (read_reg_single(REG_LR_OPMODE) & RFLR_OPMODE_FREQMODE_ACCESS_MASK) | RFLR_OPMODE_FREQMODE_ACCESS_HF);
	set_timeout(TIMEOUT);

	write_reg_single(REG_LR_DIOMAPPING1, 0x00);
	set_paboost();
	set_modemconfig2_rxpayloadcrcon(1);
	set_modemconfig2_sf();
}

void set_frequency(U32 frequency)
{
	U32 frf = frequency / FSTEP;
	write_reg_single(REG_LR_FRFMSB, (frf & 0xFFFFFF) >> 16);
	write_reg_single(REG_LR_FRFMID, (frf & 0xFFFF) >> 8);
	write_reg_single(REG_LR_FRFLSB, frf & 0xFF);
}

void set_operation_mode(U8 mode)
{
	write_reg_single(REG_LR_OPMODE, (read_reg_single(REG_LR_OPMODE) & RFLR_OPMODE_MASK) | mode);
}

void set_paboost()
{
	write_reg_single(REG_LR_PACONFIG, (read_reg_single(REG_LR_PACONFIG) & RFLR_PACONFIG_PASELECT_MASK) | RFLR_PACONFIG_PASELECT_PABOOST);
}

void set_modemconfig2_rxpayloadcrcon(U8 crc_stat)
{
	if (crc_stat) write_reg_single(REG_LR_MODEMCONFIG2, (read_reg_single(REG_LR_MODEMCONFIG2) & RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK) | RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON);
	else write_reg_single(REG_LR_MODEMCONFIG2, (read_reg_single(REG_LR_MODEMCONFIG2) & RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK) | RFLR_MODEMCONFIG2_RXPAYLOADCRC_OFF);
}

void set_modemconfig2_sf()
{
	write_reg_single(REG_LR_MODEMCONFIG2, (read_reg_single(REG_LR_MODEMCONFIG2) & RFLR_MODEMCONFIG2_SF_MASK) | RFLR_MODEMCONFIG2_SF_7);
}

void set_txbaseaddr(U8 addr)
{
	write_reg_single(REG_LR_FIFOTXBASEADDR, addr);
}

void set_rxbaseaddr(U8 addr)
{
	write_reg_single(REG_LR_FIFORXBASEADDR, addr);
}

void set_timeout(U16 symbTimeout)
{
	write_reg_single(REG_LR_MODEMCONFIG2, (read_reg_single(REG_LR_MODEMCONFIG2) & RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK) | (symbTimeout >> 8) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK);
	write_reg_single(REG_LR_SYMBTIMEOUTLSB, (uint8_t)(symbTimeout & 0xFF));
}

void idle_mode(void) {
	write_reg_single(REG_LR_OPMODE, (read_reg_single(REG_LR_OPMODE) & RFLR_OPMODE_MASK) | RFLR_OPMODE_STANDBY);
}

void Tx()
{
	write_reg_single(REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
		RFLR_IRQFLAGS_RXDONE |
		RFLR_IRQFLAGS_PAYLOADCRCERROR |
		RFLR_IRQFLAGS_VALIDHEADER |
		//RFLR_IRQFLAGS_TXDONE |
		RFLR_IRQFLAGS_CADDONE |
		RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
		RFLR_IRQFLAGS_CADDETECTED);

	// DIO0=TxDone
	write_reg_single(REG_LR_DIOMAPPING1, (read_reg_single(REG_LR_DIOMAPPING1) & RFLR_DIOMAPPING1_DIO0_MASK) | RFLR_DIOMAPPING1_DIO0_01);

	set_operation_mode(RFLR_OPMODE_TRANSMITTER);
}

void Send(U8* buffer, U8 size)
{
	// Initializes the payload size
	write_reg_single(REG_LR_PAYLOADLENGTH, size);

	// Full buffer used for Tx
	write_reg_single(REG_LR_FIFOTXBASEADDR, 0);
	write_reg_single(REG_LR_FIFOADDRPTR, 0);

	// FIFO operations can not take place in Sleep mode
	if ((read_reg_single(REG_LR_OPMODE) & ~RFLR_OPMODE_MASK) == RFLR_OPMODE_SLEEP)
	{
		idle_mode();
		wait_ms(1);
	}
	// write_reg_single payload buffer
	WriteFifo(buffer, size);
	Tx();
}

void Rx()
{
	write_reg_single(REG_LR_INVERTIQ, ((read_reg_single(REG_LR_INVERTIQ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK) | RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_OFF));
	write_reg_single(REG_LR_INVERTIQ2, RFLR_INVERTIQ2_OFF);
	//write_reg_single(REG_LR_DETECTOPTIMIZE, read_reg_single(REG_LR_DETECTOPTIMIZE) & 0x7F);

	write_reg_single(REG_LR_IRQFLAGSMASK, //RFLR_IRQFLAGS_RXTIMEOUT |
									  //RFLR_IRQFLAGS_RXDONE |
									  //RFLR_IRQFLAGS_PAYLOADCRCERROR |
		RFLR_IRQFLAGS_VALIDHEADER |
		RFLR_IRQFLAGS_TXDONE |
		RFLR_IRQFLAGS_CADDONE |
		RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
		RFLR_IRQFLAGS_CADDETECTED);

	// DIO0=RxDone
	write_reg_single(REG_LR_DIOMAPPING1, (read_reg_single(REG_LR_DIOMAPPING1) & RFLR_DIOMAPPING1_DIO0_MASK) | RFLR_DIOMAPPING1_DIO0_00);
	write_reg_single(REG_LR_DIOMAPPING1, (read_reg_single(REG_LR_DIOMAPPING1) & RFLR_DIOMAPPING1_DIO1_MASK) | RFLR_DIOMAPPING1_DIO1_00);

	write_reg_single(REG_LR_FIFORXBASEADDR, 0);
	write_reg_single(REG_LR_FIFOADDRPTR, 0);

	memset(rxtxbuf, 0, RX_BUFFER_SIZE);

	set_operation_mode(RFLR_OPMODE_RECEIVER_SINGLE);
}

void WriteFifo(U8* buffer, U8 size)
{
	write_reg_mul(0, buffer, size);
}

void ReadFifo(U8* buffer, U8 size)
{
	read_reg_mul(0, buffer, size);
}
