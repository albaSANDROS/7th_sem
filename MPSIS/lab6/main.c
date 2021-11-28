#include <msp430.h>
#include <math.h>
#include "CTS_Layer.h"
#include "HAL_PMM.h"

///////////////////////////////////////////////////////////////////////

#define off_led_1 {P1OUT &= ~BIT0;}
#define on_led_1 {P1OUT |= BIT0;}
#define on_led_7 { P1OUT |= BIT4; }
#define off_led_7 { P1OUT &= ~BIT4; }
#define on_led_8 { P1OUT |= BIT5; }
#define off_led_8 {P1OUT &= ~BIT5;}
#define toggle_led_7 { P1OUT ^= BIT4; }
#define toggle_led_8 { P1OUT ^= BIT5; }
//
//////////////////////////////////////////////////////////////////////

void write_cmds(int *cmds, int num) {
    P7OUT &= ~BIT4;
    P5OUT &= ~BIT6;

    int i;
    __bis_SR_register(GIE);
    for (i = 0; i < num; i++) {
        if (!(UCB1IFG&UCTXIFG)) {
            __bis_SR_register(LPM0_bits);
        }

        UCB1TXBUF = cmds[i];
    }
    __bic_SR_register(GIE);
    while (UCB1STAT & UCBUSY);

    P7OUT |= BIT4;
}

void init_LCD(){
    P7SEL &= ~BIT4;
    P7DIR |= BIT4;
    P7OUT |= BIT4;

    P5DIR |= BIT6;
    P5SEL |= BIT6;
    P5OUT &= ~BIT6;

    P4SEL |= BIT1;
    P4DIR |= BIT1;

    P4SEL |= BIT3;
    P4DIR |= BIT3;

    P7SEL &= ~BIT6;
    P7DIR |= BIT6;
    P7OUT |= BIT6;

    P5DIR |= BIT7;
    P5SEL &= BIT7;
    P5OUT &= BIT7;
    __delay_cycles(25000);
    P5OUT |= BIT7;
    __delay_cycles(125000);

    UCB1CTL1 = UCSSEL_2 | UCSWRST;
    UCB1CTL0 = UCCKPH | UCMSB | UCMST | UCMODE_0 | UCSYNC;
    UCB1BR0 = 0x02;
    UCB1BR1 = 0;
    UCB1IE = UCTXIE;
    UCB1IFG &= ~UCTXIFG;
    UCB1CTL1 &= ~UCSWRST;

    int init_cmd[] = {0x40,
                      0xA0,
                      0xC0,
                      0xA4,
                      0xA6,
                      0xA2,
                      0x2F,
                      0x27, 0x81, 0x10,
                      0xFA, 0x90,
                      0xC8,
                      0xAF};
    write_cmds(init_cmd, 14);
}

void clear_LCD(){
    int page, j;
    for (page = 0; page < 8; page++) {

        int cmd[1];
        cmd[0] = 0xB0 + page;
        write_cmds(cmd, 1);

        int address[2];
        address[0] = 0x0E;
        address[1] = 0x11;
        write_cmds(address, 2);

        for (j = 30; j <= 131; j++){
            P7OUT &= ~BIT4;
            P5OUT |= BIT6;

            __bis_SR_register(GIE);
            if (!(UCB1IFG&UCTXIFG)) {
                __bis_SR_register(LPM0_bits);
            }

            UCB1TXBUF = 0x00;

            __bic_SR_register(GIE);

            while (UCB1STAT & UCBUSY);

            P7OUT |= BIT4;
        }
    }
}

int digits[13][4] = {
        {0x7F, 0x41, 0x41, 0x7F}, //0
        {0x0C, 0x06, 0x7F, 0x7F}, //1
        {0x71, 0x49, 0x49, 0x47}, //2
        {0x41, 0x49, 0x49, 0x7F}, //3
        {0x0F, 0x08, 0x08, 0x7F}, //4
        {0x47, 0x49, 0x49, 0x71}, //5
        {0x7F, 0x49, 0x49, 0x79}, //6
        {0x41, 0x31, 0x09, 0x07}, //7
        {0x7F, 0x49, 0x49, 0x7F}, //8
        {0x46, 0x49, 0x49, 0x3F}, //9
        {0x08, 0x1C, 0x1C, 0x08}, //+
        {0x08, 0x08, 0x08, 0x08}, //-
        {0x00, 0x00, 0x00, 0x00}, // blank
};

inline void select_start_position(const int* const address) {
    int cmd[1];
    cmd[0] = 0xB0;
    write_cmds(cmd, 1);

    write_cmds(address, 2);
}

#pragma vector = USCI_B1_VECTOR
__interrupt void lcd_spi_busy_interrupt(void)
{
    UCB1IFG &= ~UCTXIFG;
    __bic_SR_register_on_exit(LPM0_bits);
}

void write_number_to_LCD(int number){
    int i;
    for (i = 0; i < 4; i++){
        P7OUT &= ~BIT4;
        P5OUT |= BIT6;

        __bis_SR_register(GIE);
        if (!(UCB1IFG&UCTXIFG)) {
            __bis_SR_register(LPM0_bits);
        }

        UCB1TXBUF = digits[number][i];

        __bic_SR_register(GIE);

        while (UCB1STAT & UCBUSY);

        P7OUT |= BIT4;
    }

    P7OUT &= ~BIT4;
    P5OUT |= BIT6;

    __bis_SR_register(GIE);
    if (!(UCB1IFG&UCTXIFG)) {
        __bis_SR_register(LPM0_bits);
    }

    UCB1TXBUF = 0x00;

    __bic_SR_register(GIE);

    while (UCB1STAT & UCBUSY);

    P7OUT |= BIT4;
}

inline void print_number(signed long int number) {
    char numbers[5] = {};
    int number_index;
    on_led_8;
    if (number < 0) {
        number *= (-1);
        write_number_to_LCD(11);
    }
    else {
        write_number_to_LCD(10);
    }

    for (number_index = 4; number_index; number_index--) {
        numbers[number_index] = number % 10;
        number /= 10;
    }

    for (number_index = 0; number_index < 5; number_index++) {
        write_number_to_LCD(numbers[number_index]);
    }
    off_led_8;
}

///////////////////////////////////////////////////////

#pragma vector = ADC12_VECTOR
__interrupt void ADC_interrupt() {
    ADC12IE &= ~ADC12IE10;

	int address[2];
	long signed int value = ADC12MEM10;

	//value = value/4096*300+585;
	value = value*0.122 - 200;
	value -= 40; // ���-�� ���� ���������� ,x,
    address[0] = 0x0E;
    address[1] = 0x11;
    select_start_position(address);
    print_number(value);

    ADC12IE |= ADC12IE10;
    ADC12IFG &= ~ADC12IFG10;
}

struct Element* keypressed;
//
/////////////////////////////////////////////////////////////
//
int main(void)
{
    // Âûêëþ÷èòü âîò÷äîã

    WDTCTL = WDTPW | WDTHOLD;

    // Èíèöèàëèçàöèÿ äèîäîâ 6-7 è êíîïîê S1-2
    // ãåíåðàòîðà FLL è òàéìåðà A0

    P1DIR |= BIT1; // LED 4
    P1SEL &= ~BIT1;
    P1OUT &= ~BIT1;

    P1DIR |= BIT4; // LED 7
    P1SEL &= ~BIT4;
    P1OUT &= ~BIT4;

    P1DIR |= BIT5; // LED 8
    P1SEL &= ~BIT5;
    P1OUT &= ~BIT5;

    P8DIR = 0xFF;
    P8OUT = 0;

    init_LCD();
    clear_LCD();

    SetVCore(0x03);

    UCSCTL3 = SELREF_2;                       // Set DCO FLL reference = REFO
    UCSCTL4 |= SELA_2;                        // Set ACLK = REFO

    // __bis_SR_register(SCG0);                  // Disable the FLL control loop
    UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
    UCSCTL1 = DCORSEL_7;
    // Select DCO ran1ge 50MHz operation
    UCSCTL2 = FLLD_1 + 762;

    // ADC
    REFCTL0 &= ~REFMSTR;
    ADC12MCTL10 = ADC12SREF_1 | ADC12INCH_10 | ADC12EOS;
    ADC12CTL0 = ADC12REFON | ADC12ON | ADC12SHT1_4 | ADC12MSC;
    ADC12CTL1 = ADC12SHP | ADC12CONSEQ_1 | ADC12SHS_0;
    ADC12CTL1 |= ADC12CSTARTADD_10 | ADC12SSEL_0 | ADC12DIV_0;
    ADC12CTL2 &= ~ADC12TCOFF;
    ADC12CTL2 |= ADC12RES_2;
    ADC12IE |= ADC12IE10;
    ADC12IFG &= ~ADC12IFG10;

    __bis_SR_register(GIE);

    ADC12CTL0 |= ADC12ENC;
    ADC12CTL0 |= ADC12SC;

    TI_CAPT_Init_Baseline(&slider);
    TI_CAPT_Update_Baseline(&slider, 5);

    while (1) {
        keypressed = (struct Element*) TI_CAPT_Buttons(&slider);

        __no_operation();
        if (keypressed) {
            on_led_7;
            if (!(ADC12CTL1 & ADC12BUSY)) {
                ADC12CTL0 |= ADC12SC;
            }
        }
        else {
            off_led_7;
        }
        __delay_cycles(900000);
    }

    return 0;
}
