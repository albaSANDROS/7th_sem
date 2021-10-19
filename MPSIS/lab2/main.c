#include <msp430.h>

int LPM2_mode = 0;
int change_frequency_mode = 0;

void sleep(int time) {
    volatile int i = 0;
    for (i = 0; i < time; i++) {}
}

#pragma vector = PORT1_VECTOR
__interrupt void button1(void) {
    sleep(1000);

    if (LPM2_mode)
    {
        _bic_SR_register_on_exit(LPM2_bits);
        LPM2_mode = 0;
    } else
    {
        _bis_SR_register_on_exit(LPM2_bits); //вход в режим энергносбережения
        LPM2_mode = 1;
    }

    P1IFG &= ~BIT7;
}

#pragma vector = PORT2_VECTOR
__interrupt void button2(void) {

    if (change_frequency_mode)
    {
        sleep(1000);
        UCSCTL4 = SELM__DCOCLK;
        UCSCTL5 = DIVM__4;
        change_frequency_mode = 0;
    } else
    {
        sleep(300);
        UCSCTL4 = SELM__REFOCLK;
        UCSCTL5 = DIVM__16; //источник
        change_frequency_mode = 1;
    }

    P2IFG &= ~BIT2;
}



int main(void) {
    WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer

    //init buttons
    P1DIR &= ~BIT7;
    P1OUT |= BIT7;
    P1REN |= BIT7;

    P2DIR &= ~BIT2;
    P2OUT |= BIT2;
    P2REN |= BIT2;

    __bis_SR_register(GIE);

    //interrupts on buttons
    P1IES |= BIT7;
    P1IFG &= ~BIT7;
    P1IE |= BIT7;

    P2IES |= BIT2;
    P2IFG &= ~BIT2;
    P2IE |= BIT2;

    //pins for MCLK reading
    P7DIR |= BIT7;
    P7SEL |= BIT7;

    //CLK registers managment
    UCSCTL1 = 0;
    UCSCTL2 = 0;
    UCSCTL3 = 0;
    UCSCTL4 = 0;
    UCSCTL5 = 0;

    //dclock 1.23Mhz
    UCSCTL1 = DCORSEL_1;
    UCSCTL2 = FLLD__1 | FLLN5 | FLLN0 | FLLN2; 
    UCSCTL3 = SELREF__XT1CLK | FLLREFDIV__1;

    
    UCSCTL4 = SELM__DCOCLK;
    
    //MCLK div
    UCSCTL5 = DIVM__1;                      // тут менять на 1 и 4

    __no_operation();

    return 0;
}
