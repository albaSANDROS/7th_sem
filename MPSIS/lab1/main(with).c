#include <msp430.h> 

int b1_prev_state = BIT7;
int b2_prev_state = BIT2;
int l1_state = 0;

void sleep() {
	volatile int i = 0;
	for	(i = 0; i < 1000; i++) {}
}

#pragma vector = PORT1_VECTOR
__interrupt void button1(void) { //LEDB S1 using
	sleep();

	int b1_cur_state = (P1IN & BIT7);

	if (b1_prev_state == 0 && b1_cur_state == BIT7) {
   		P1OUT |= BIT2;
   	}

   	if (b1_prev_state == BIT7 && b1_cur_state == 0 && b2_prev_state == 0) {
   	    P1OUT &= ~BIT2;
   	}

   	b1_prev_state = b1_cur_state;

	P1IES ^= BIT7; //xor to BIT7 to get changes
	P1IFG = 0;
}

#pragma vector = PORT2_VECTOR
__interrupt void button2(void) {
	sleep();

	int b2_cur_state = (P2IN & BIT2);

	if (b1_prev_state == BIT7 && b2_prev_state == 0 && b2_cur_state == BIT2) {
			if (l1_state == 0){
		    	P1OUT |= BIT3;
		    	l1_state = 1;
		    } else {
		    	if (l1_state == 1){
		    		P1OUT &= ~BIT3;
		    		l1_state = 0;
		    	}
		    }
		}

	b2_prev_state = b2_cur_state;

	P2IES ^= BIT2;
	P2IFG = 0;
}



int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    //LED1 - 1.3
    //LED2 - 1.2

    P1DIR = BIT2 | BIT3;
    P1OUT = 0;

    P1REN |= BIT7;
    P1OUT |= BIT7;

    P2REN |= BIT2;
    P2OUT |= BIT2;

    P1IES |= BIT7;
    P2IES |= BIT2;

    P1IFG = 0;
    P2IFG = 0;

    P1IE |= BIT7;
    P2IE |= BIT2;

    __bis_SR_register(LPM0_bits + GIE);

    __no_operation();

    return 0;
}

