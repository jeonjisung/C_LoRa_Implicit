#include "mbed.h"
#include "lora.h"

Serial pc(PA_9, PA_10);

InterruptIn DIO0(PB_6);
InterruptIn DIO1(PB_7);

u8 writebuf[] = "asdfg";
int count = 0;

int main()
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
        printf("diomap: 0x%2X\r\n", read_reg_single(REG_LR_DIOMAPPING1));
        wait(1);
        printf("write:%s\r\n", writebuf);
        Send(writebuf, 5);
        wait(0.1);
    }
}