// uncorrect var!

#include <msp430.h> 


int LED4 = 0; //диод 4 : 0 - не горит 1 -горит
int LED5 = 0; //диод 5 : 0 - не горит 1 -горит

int buttonS1 = 0;
int buttonS2 = 0;
// 1 - нажата
// 0 - не нажата


// обработчик прерываний
#pragma vector = PORT1_VECTOR  //для порта 1
__interrupt void buttonPush1(void) {




	if (buttonS1 == 0) { //кнока нажата
		if(LED5 == 0 && buttonS2 == 0){
			P1OUT |= BIT2;
			LED5 = 1;
		}
        P1IES &= ~BIT7; //реагирование только на фронт
        buttonS1 = 1;

    } else {

        P1IES |= BIT7; //реагирование только на спад
        buttonS1 = 0;
    }



   // volatile int i = 0;
   // for (i = 0; i < 1000; i++) {}
    P1IFG = 0;
}

#pragma vector = PORT2_VECTOR  //для порта 2
__interrupt void buttonPush2(void) {

	if (buttonS2 == 0) { //кнока нажата
        P2IES &= ~BIT2; //реагирование только на фронт
        buttonS2 = 1;
        if(buttonS1 == 1 && LED4 == 0){
        	P1OUT |= BIT1;
        	LED4 = 1;
        }
    } else {
    	if(LED5 == 1){
    		P1OUT &= ~BIT2;
    		LED5 = 0;
    	}

        P2IES |= BIT2; //реагирование только на спад
        buttonS2 = 0;

        if(buttonS1 == 1 && LED4 == 1){
        	 P1OUT &= ~BIT1;
        	 LED4 = 0;
        }
    }

  //  volatile int i = 0;
  //  for (i = 0; i < 1000; i++) {}
    P2IFG = 0;
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer (отключение сторожевого таймера)

    P1OUT = 0;
    P2OUT = 0;

    P1SEL = 0;
    P2SEL = 0;

	P1DIR |= (BIT1 | BIT2);	// 1 - выход 0 - вход (2 дтода 4 и 5 - 1.1 и 1.2)

    P1OUT |= BIT7;
    P2OUT |= BIT2;

    P1REN |= BIT7; // от помех на кнопке S1
    P2REN |= BIT2; // от помех на кнопке S2

	P1IE |= BIT7; // разрешение прерывания S1
	P2IE |= BIT2; // разрешение прерывания S2

    __bis_SR_register(GIE);

    P1IE |= BIT7;
    P2IE |= BIT2;
    P1IES |= BIT7;
    P2IES |= BIT2;
    P1IFG = 0;
    P2IFG = 0;

    __no_operation();

	return 0;
}
