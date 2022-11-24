#include "mbed.h"
#include "lora.h"

Serial pc(PA_9, PA_10);

InterruptIn DIO0(PB_6);
InterruptIn DIO1(PB_7);

u8 writebuf[] = "bbbbb";
int count = 0;

void main()
{
    DIO0.mode(PullDown);
    DIO1.mode(PullDown);

    DIO0.rise(&RxTxDone_interrupt);
    DIO1.rise(&RxTimeout_interrupt);
    pc.baud(115200);

    spi_init();
    lora_init();

    while (1)
    {   
        memset(rxtxbuf, 0, RX_BUFFER_SIZE);
        Rx();
        printf("%s\r\n", rxtxbuf);
        printf("diomap : 0x%2x\r\n", read_reg_single(REG_LR_DIOMAPPING1));
        wait(1);
        printf("write : %s\r\n", writebuf);
        Send(writebuf, 5);
        wait(0.1);
    }
}

void RxTimeout_interrupt()
{
    printf("IRQ : 0x%2x\r\n", read_reg_single(REG_LR_IRQFLAGS));
    printf("-----TIMEOUT-----\r\n");
    write_reg_single(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXTIMEOUT);
    idle_mode();
}

void RxTxDone_interrupt()
{
    printf("IRQ : 0x%2x\r\n", read_reg_single(REG_LR_IRQFLAGS));
    if (read_reg_single(REG_LR_IRQFLAGS) == 0x40)
    {
        printf("-----RXDONE-----\r\n");
        write_reg_single(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE);
        read_fifo(rxtxbuf, read_reg_single(REG_LR_RXNBBYTES));
        printf("[%s]\r\n", rxtxbuf);
    }
    else if (read_reg_single(REG_LR_IRQFLAGS) == 0x08)
    {
        write_reg_single(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE);
        printf("-----TXDONE-----\r\n");
    }
    idle_mode();
}