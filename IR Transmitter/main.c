//Jacob Foster
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
#define CMD_PUL 0xFA
#define CMD_PUR 0xFB
#define CMD_RMP 0xFC

#define BTN_FWD BIT2 //2.2
#define BTN_BCK BIT1 //2.1
#define BTN_LFT BIT0 //2.0
#define BTN_RGT BIT5 //1.5
#define BTN_PUL BIT4 //2.4
#define BTN_PUR BIT3 //2.3
#define BTN_RMP BIT5 //2.5

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
    WDTCTL = WDTPW | WDTHOLD;

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

void GPIO_init(void){ //internal pull ups enabled on buttons, this function can be cleaned but i like the way its laid out
    P2DIR &= ~BTN_FWD;
    P2REN |= BTN_FWD;
    P2OUT |= BTN_FWD;

    P2DIR &= ~BTN_BCK;
    P2REN |= BTN_BCK;
    P2OUT |= BTN_BCK;

    P2DIR &= ~BTN_LFT;
    P2REN |= BTN_LFT;
    P2OUT |= BTN_LFT;

    P1DIR &= ~BTN_RGT;
    P1REN |= BTN_RGT;
    P1OUT |= BTN_RGT;

    P2DIR &= ~BTN_PUR;
    P2REN |= BTN_PUR;
    P2OUT |= BTN_PUR;

    P2DIR &= ~BTN_PUL;
    P2REN |= BTN_PUL;
    P2OUT |= BTN_PUL;

    P2DIR &= ~BTN_RMP;
    P2REN |= BTN_RMP;
    P2OUT |= BTN_RMP;
}

int main(void)
{
    uart_init();
    timer_init();
    GPIO_init();

    while (1)
    { //maybe a better way but this works without issues so bleah
        if(!(P2IN & BTN_FWD)){
            uart_send_command(CMD_FWD);
        } else if (!(P2IN & BTN_BCK)){
            uart_send_command(CMD_BCK);
        } else if (!(P2IN & BTN_LFT)){
            uart_send_command(CMD_LFT);
        } else if (!(P1IN & BTN_RGT)){
            uart_send_command(CMD_RGT);
        } else if (!(P2IN & BTN_PUL)){
            uart_send_command(CMD_PUL);
        } else if (!(P2IN & BTN_PUR)){
            uart_send_command(CMD_PUR);
        } else if (!(P2IN & BTN_RMP)){
            uart_send_command(CMD_RMP);
        } else {
            uart_send_command(CMD_STOP);
        }
        
        __delay_cycles(100000); //changing this may change speed?
    }
}
