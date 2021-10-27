#include <msp430.h>

int currentTimer = 0; // 0 - TA2, 1 - WD
int interruptsWDTCount = 0;
int currentLed = 2; // 0 - led6, 1 - led7, 2 - led8
const int DELAY = 1500;

////// for s1 delay /////////////////////
void startTimerA1() {
    TA1CCTL0 = CCIE; // allow interruptions from TA1
    TA1CCR0 = DELAY; //
    TA1EX0 = TAIDEX_7; // 8
    TA1CTL = TASSEL_2 | ID_3 | MC_1 | TACLR; // id = 8
}

void stopTimerA1() {
    TA1CCTL0 &= ~CCIE;
    TA1CTL = MC_0; // stop timer
}
/////////////////////////////////////////

////// task /////////////////////////////
void startTimerA2() {
    TA2CCTL0 = CCIE; // allow interruptions from TA2
    TA2CCR0 = 4476; // ~0.3sec
    TA2EX0 = TAIDEX_7; // 8
    TA2CTL = TASSEL_2 | ID_3 | MC_1 | TACLR; // id = 8
}

void stopTimerA2() {
    TA2CCTL0 &= ~CCIE;
    TA2CTL = MC_0; // stop timer
}
/////////////////////////////////////////

////// for s2 delay /////////////////////
void startTimerB0() {
    TB0CCTL0 = CCIE; // allow interruptions from TB0
    TB0CCR0 = DELAY; //
    TB0EX0 = TAIDEX_7; // 8
    TB0CTL = TASSEL_2 | ID_3 | MC_1 | TACLR; // id = 8
}

void stopTimerB0() {
    TB0CCTL0 &= ~CCIE;
    TB0CTL = MC_0; // stop timer
}
/////////////////////////////////////////

////// task /////////////////////////////
void startWatchDogTimer() {
    SFRIE1 |= WDTIE; // enable interruptions from WDT
    WDTCTL = WDTPW | WDTTMSEL | WDTCNTCL | WDTSSEL_0 | WDTIS_4;
}

void stopWatchDogTimer() {
    SFRIE1 &= ~WDTIE;
    WDTCTL = WDTPW | WDTHOLD;
}
//////////////////////////////////////////

void shutNextLed() {
    // shut down chosen LED, activate the others
    switch(currentLed) {
    case 0:
        P1OUT |= BIT3;
        P1OUT &= ~BIT4;
        P1OUT |= BIT5;
        currentLed = 1;
        break;
    case 1:
        P1OUT |= BIT3;
        P1OUT |= BIT4;
        P1OUT &= ~BIT5;
        currentLed = 2;
        break;
    case 2:
        P1OUT &= ~BIT3;
        P1OUT |= BIT4;
        P1OUT |= BIT5;
        currentLed = 0;
        break;
    }
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void TIMER_1 (void) {
    // s1 pressed
    if((P1IN & BIT7) == 0) {
        shutNextLed();
        if(currentTimer == 0){
            startTimerA2();
        } else {
            startWatchDogTimer();
        }
    }

    stopTimerA1();
}

#pragma vector = TIMER2_A0_VECTOR
__interrupt void TIMER_2 (void) {
    P1OUT |= BIT3;
    P1OUT |= BIT4;
    P1OUT |= BIT5;
    stopTimerA2();
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void TIMER_3 (void) {
    // s2 pressed
    if((P2IN & BIT2) == 0){
        if(currentTimer == 0) { // a2 is active
            stopTimerA2();
            P8OUT &= ~BIT1;

            P8OUT |= BIT2;
            currentTimer = 1;
        } else {
            stopWatchDogTimer();
            P8OUT &= ~BIT2;

            P8OUT |= BIT1;
            currentTimer = 0;
        }
    }

    stopTimerB0();
}

#pragma vector = WDT_VECTOR
__interrupt void WDT_interrupt(void) {
    interruptsWDTCount++;
    if (interruptsWDTCount >= 10) {
        interruptsWDTCount = 0;
        P1OUT |= BIT3;
        P1OUT |= BIT4;
        P1OUT |= BIT5;
        stopWatchDogTimer();
    }
}

#pragma vector = PORT1_VECTOR
__interrupt void buttonS1(void)
{
    startTimerA1();

    P1IFG = 0;
    P1IES ^= BIT7;
}

#pragma vector = PORT2_VECTOR
__interrupt void buttonS2(void)
{
    startTimerB0();

    P2IFG = 0;
    P2IES ^= BIT2;
}


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;

    // led2 - A2
    P8DIR |= BIT1;
    P8OUT |= BIT1;

    // led3 - WDT
    P8DIR |= BIT2;
    P8OUT &= ~BIT2;

    // led5
    P1DIR |= BIT2;
    P1OUT &= ~BIT2;
    P1SEL |= BIT2; // blinking

    // led6
    P1DIR |= BIT3;
    P1OUT |= BIT3;

    // led7
    P1DIR |= BIT4;
    P1OUT |= BIT4;

    // led8
    P1DIR |= BIT5;
    P1OUT |= BIT5;

    // butt1
    P1DIR &= ~BIT7;
    P1OUT |= BIT7;
    P1REN |= BIT7;
    P1IE |= BIT7;
    P1IES |= BIT7;
    P1IFG = 0;

    // butt2
    P2DIR &= ~BIT2;
    P2OUT |= BIT2;
    P2REN |= BIT2;
    P2IE |= BIT2;
    P2IES |= BIT2;
    P2IFG = 0;

    // timer a0
    TA0CCR0 = 32768; // 2sec
    TA0CCR1 = 16384; // должно быть CCR22 CCTL2, но в нем не работает 5й LED
    TA0CCTL1 = OUTMOD_6; // перекл-е/уст-ка
    TA0EX0 = TAIDEX_7; // 8
    TA0CTL = TASSEL_2 | ID_3 | MC_1 | TACLR; // 8

    __bis_SR_register(GIE);
    __no_operation();

    return 0;
}
