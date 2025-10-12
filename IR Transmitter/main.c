//IR transmitter using UART
//2400 Baud, 8-bit, no parity
//low Buad to make it easier to capture full signal from a distance
//P1.2 is Tx
//P1.6 used for IR LED bc lights up onboard LED as well for debuggin

//Tx is used to show when the signal is 'active', thats when the 38kHz signal will actually be transmitted

//talk to tarter to get wiring diagram again

#include <msp430.h>
#include <stdint.h>

#define CMD_STOP    0x00
#define CMD_FWD     0x01
#define CMD_BCK     0x02
#define CMD_LFT     0x03
#define CMD_RGT     0x04

void uart_init(void)
{
    //config UART pins
    P1SEL |= BIT2; //P1.2 is Tx
    P1SEL2 |= BIT2;

    //config UART
    UCA0CTL1 |= UCSWRST; //put eUSCI in reset
    UCA0CTL1 |= UCSSEL_2; //use SMCLK

    //set baud rate to 2400
    UCA0BR0 = 104; //16 MHz / 2400 = 6666.67, UCA0BR0 = 6666 & 0xFF
    UCA0BR1 = 0; //UCA0BR1 = 6666 >> 8
    UCA0MCTL = UCBRS_1 | UCBRF_0 | UCOS16; //modulation UCBRSx = 1, UCBRFx = 0, Oversampling enabled

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
    WDTCTL = WDTPW | WDTHOLD; //stop watchdog

    uart_init();
    timer_init();

    while (1)
    {
        //test, fwd for eternity
        uart_send_command(CMD_FWD);
        __delay_cycles(20000);
    }

}
