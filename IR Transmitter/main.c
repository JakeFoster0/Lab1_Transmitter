//IR transmitter using UART
//2400 Baud, 8-bit, no parity
//low Buad to make it easier to capture full signal from a distance
//P1.2 is Tx
//P1.6 used for IR LED bc lights up onboard LED as well for debuggin

//Tx is used to show when the signal is 'active', thats when the 38kHz signal will actually be transmitted

#include <msp430.h>
#include <stdint.h>

#define CMD_STOP 0xFF
#define CMD_FWD 0xF0
#define CMD_BCK 0xF1
#define CMD_LFT 0xF2
#define CMD_RGT 0xF3

void uart_init(void)
{
    //config UART pins
    P1SEL |= BIT2; //P1.2 is Tx
    P1SEL2 |= BIT2;

    //config UART
    UCA0CTL1 |= UCSWRST; //put eUSCI in reset
    UCA0CTL1 |= UCSSEL_2; //use SMCLK

    //set baud rate to 2400
    UCA0BR0 = 26;
    UCA0BR1 = 0;
    UCA0MCTL = UCOS16 | (0xB << 4) | (0x08 << 8);
    UCA0CTL1 &= ~UCSWRST; //reset
}

void timer_init(void)
{
    //sets 1.6 as peripheral to output 38kHz signal
    P1DIR |= BIT6;
    P1SEL |= BIT6;

    //PWM setup for 38kHz at 50% duty cycle
    TA0CCR0 = 26 - 1;
    TA0CCTL1 = OUTMOD_7;
    TA0CCR1 = 13;
    TA0CTL = TASSEL_2 | MC_1;
}

void uart_send_command(uint8_t cmd)
{
    while (!(IFG2 & UCA0TXIFG)); //wait for TX buffer
    UCA0TXBUF = cmd; //send command byte
}

int main(void)
{
    uart_init();
    timer_init();

    while (1)
    {
        //test
        uart_send_command(CMD_BCK);
        __delay_cycles(200000);
        uart_send_command(CMD_LFT);
        __delay_cycles(500000);

        uart_send_command(CMD_FWD);
        __delay_cycles(200000);
        uart_send_command(CMD_RGT);
        __delay_cycles(500000);
    }
}
