#include <msp430.h>
#include <math.h>

typedef unsigned char uint8_t;

#define SET_COLUMN_ADDRESS_LSB        0x00
#define SET_COLUMN_ADDRESS_MSB        0x10
#define SET_PAGE_ADDRESS              0xB0

#define SET_SEG_DIRECTION             0xA0
#define SET_COM_DIRECTION             0xC0

#define SET_POWER_CONTROL             0x2F
#define SET_SCROLL_LINE               0x40
#define SET_VLCD_RESISTOR_RATIO       0x27
#define SET_ELECTRONIC_VOLUME_MSB     0x81
#define SET_ELECTRONIC_VOLUME_LSB     0x0F
#define SET_ALL_PIXEL_ON              0xA4
#define SET_INVERSE_DISPLAY           0xA6
#define SET_DISPLAY_ENABLE            0xAF
#define SET_LCD_BIAS_RATIO            0xA2
#define SET_ADV_PROGRAM_CONTROL0_MSB  0xFA
#define SET_ADV_PROGRAM_CONTROL0_LSB  0x90

#define NONE						0
#define READ_Y_AXIS_DATA 			0x1C

#define CD              BIT6
#define CS              BIT4

uint8_t Dogs102x6_initMacro[] = {
	SET_SCROLL_LINE,
	SET_SEG_DIRECTION,
	SET_COM_DIRECTION,
	SET_ALL_PIXEL_ON,
	SET_INVERSE_DISPLAY,
	SET_LCD_BIAS_RATIO,
	SET_POWER_CONTROL,
	SET_VLCD_RESISTOR_RATIO,
	SET_ELECTRONIC_VOLUME_MSB,
	SET_ELECTRONIC_VOLUME_LSB,
	SET_ADV_PROGRAM_CONTROL0_MSB,
	SET_ADV_PROGRAM_CONTROL0_LSB,
	SET_DISPLAY_ENABLE,
	SET_PAGE_ADDRESS,
	SET_COLUMN_ADDRESS_MSB,
	SET_COLUMN_ADDRESS_LSB
};

// 10000 * 9.81 -- convert to 10^4 m per sec^2
long int CONVERT_TO_METERS = 98100;
unsigned char whatChecking = 0;
int COLUMN_START_ADDRESS = 30;
int MAPPING_VALUES[] = { 1142, 571, 286, 143, 71, 36, 18 };
uint8_t BITx[] = { BIT6, BIT5, BIT4, BIT3, BIT2, BIT1, BIT0 };

uint8_t symbols[12][11] = {
		{0x20, 0x20, 0x20, 0x20, 0x20, 0xF8, 0x20, 0x20, 0x20, 0x20, 0x20}, // plus
		{0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00}, // minus
		{0xF8, 0xF8, 0xD8, 0xD8, 0xD8, 0xD8, 0xD8, 0xD8, 0xD8, 0xF8, 0xF8}, // num0
		{0xF8, 0xF8, 0x30, 0x30, 0x30, 0x30, 0xF0, 0xF0, 0x70, 0x70, 0x30}, // num1
		{0xF8, 0xF8, 0xC0, 0xC0, 0xC0, 0xF8, 0xF8, 0x18, 0x18, 0xF8, 0xF8}, // num2
		{0xF8, 0xF8, 0x18, 0x18, 0x18, 0xF8, 0xF8, 0x18, 0x18, 0xF8, 0xF8}, // num3
		{0x18, 0x18, 0x18, 0x18, 0xF8, 0xF8, 0xD8, 0xD8, 0xD8, 0xD8, 0xD8}, // num4
		{0xF8, 0xF8, 0x18, 0x18, 0x18, 0xF8, 0xF8, 0xC0, 0xC0, 0xF8, 0xF8}, // num5
		{0xF8, 0xF8, 0xD8, 0xD8, 0xD8, 0xF8, 0xF8, 0xC0, 0xC0, 0xF8, 0xF8}, // num6
		{0xC0, 0xC0, 0xE0, 0x70, 0x38, 0x18, 0x18, 0x18, 0x18, 0xF8, 0xF8}, // num7
		{0xF8, 0xF8, 0xD8, 0xD8, 0xD8, 0xF8, 0xD8, 0xD8, 0xD8, 0xF8, 0xF8}, // num8
		{0xF8, 0xF8, 0x18, 0x18, 0xF8, 0xF8, 0xD8, 0xD8, 0xD8, 0xF8, 0xF8}  // num9
};

int getNumberLength(long int number);
void printNumber(long int angle);
uint8_t CMA3000_writeCommand(uint8_t byte_one, uint8_t byte_two);
void CMA3000_init(void);
int calculateAngleFromProjection(double projection);
long int parseProjectionByte(uint8_t projection_byte);

void Dogs102x6_clearScreen(void);
void Dogs102x6_setAddress(uint8_t pa, uint8_t ca);
void Dogs102x6_writeData(uint8_t* sData, uint8_t i);
void Dogs102x6_writeCommand(uint8_t* sCmd, uint8_t i);
void Dogs102x6_backlightInit(void);
void Dogs102x6_init(void);

#pragma vector = PORT2_VECTOR
__interrupt void accelerometerInterrupt(void) {
	volatile uint8_t yProjectionByte = CMA3000_writeCommand(READ_Y_AXIS_DATA, NONE);

	volatile long int yAxisProjection = parseProjectionByte(yProjectionByte)-197;

	volatile long int milesPerHourSquared = (yAxisProjection) * CONVERT_TO_METERS;

	Dogs102x6_clearScreen();
	printNumber(milesPerHourSquared);

	int angle = calculateAngleFromProjection((double) yAxisProjection);

	if (90 >= angle && angle >= 0) {
		P1OUT |= BIT5;
	}
	else {
		P1OUT &= ~BIT5;
	}
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void TIMER_1 (void)
{
    if(whatChecking == 1)
    {
        if(UCA0IFG & UCTXIFG)
        {
            __bic_SR_register_on_exit(LPM0_bits + GIE);
        }
    }
    else if(whatChecking == 2)
    {
        if(UCA0IFG & UCRXIFG)
        {
            __bic_SR_register_on_exit(LPM0_bits + GIE);
        }
    }
    else if(whatChecking == 3)
    {
        if(!(UCA0STAT & UCBUSY))
        {
            __bic_SR_register_on_exit(LPM0_bits + GIE);
        }
    }
}

void printNumber(long int number) {
	int nDigits = getNumberLength(number);

	Dogs102x6_setAddress(0, COLUMN_START_ADDRESS);
	Dogs102x6_writeData(number > 0 ? symbols[0] : symbols[1], 11);

	int i = 0;
	long int divider = pow(10, nDigits - 1);

	number = fabsl(number);

	for (i = 1; i <= nDigits; i++) {
		int digit = number / divider;

		Dogs102x6_setAddress(i, COLUMN_START_ADDRESS);
		Dogs102x6_writeData(symbols[digit + 2], 11);

		number = number % divider;
		divider /= 10;
	}
}

int getNumberLength(long int number) {
	int length = 0;
	number = fabsl(number);

	if(number == 0) {
		return 1;
	}

	while(number) {
		number /= 10;
		length++;
	}

	return length;
}

void Dogs102x6_clearScreen(void)
{
	uint8_t LcdData[] = { 0x00 };
	uint8_t p, c;

	for (p = 0; p < 8; p++)
	{
		Dogs102x6_setAddress(p, 0);

		for (c = 0; c < 132; c++)
		{
			Dogs102x6_writeData(LcdData, 1);
		}
	}
}

void Dogs102x6_setAddress(uint8_t pa, uint8_t ca)
{
	uint8_t cmd[1];

	if (pa > 7)
	{
		pa = 7;
	}

	if (ca > 101)
	{
		ca = 101;
	}

	cmd[0] = SET_PAGE_ADDRESS + (7 - pa);
	uint8_t H = 0x00;
	uint8_t L = 0x00;
	uint8_t ColumnAddress[] = { SET_COLUMN_ADDRESS_MSB, SET_COLUMN_ADDRESS_LSB };

	L = (ca & 0x0F);
	H = (ca & 0xF0);
	H = (H >> 4);

	ColumnAddress[0] = SET_COLUMN_ADDRESS_LSB + L;
	ColumnAddress[1] = SET_COLUMN_ADDRESS_MSB + H;

	Dogs102x6_writeCommand(cmd, 1);
	Dogs102x6_writeCommand(ColumnAddress, 2);
}

void Dogs102x6_writeData(uint8_t* sData, uint8_t i)
{
	P7OUT &= ~CS;
	P5OUT |= CD;

	while (i)
	{
		while (!(UCB1IFG & UCTXIFG));

		UCB1TXBUF = *sData;

		sData++;
		i--;
	}

	while (UCB1STAT & UCBUSY);

	UCB1RXBUF;

	P7OUT |= CS;
}

void Dogs102x6_writeCommand(uint8_t* sCmd, uint8_t i)
{
	P7OUT &= ~CS;
	P5OUT &= ~CD;

	while (i)
	{
		while (!(UCB1IFG & UCTXIFG));

		UCB1TXBUF = *sCmd;

		sCmd++;
		i--;
	}

	while (UCB1STAT & UCBUSY);

	UCB1RXBUF;

	P7OUT |= CS;
}

void Dogs102x6_backlightInit(void)
{
	P7DIR |= BIT6;
	P7OUT |= BIT6;
	P7SEL &= ~BIT6;
}

void Dogs102x6_init(void)
{
	P5DIR |= BIT7;
	P5OUT &= BIT7;
	P5OUT |= BIT7;

	P7DIR |= CS;

	P5DIR |= CD;
	P5OUT &= ~CD;

	P4SEL |= BIT1;
	P4DIR |= BIT1;

	P4SEL |= BIT3;
	P4DIR |= BIT3;

	UCB1CTL1 = UCSSEL_2 + UCSWRST;
	UCB1BR0 = 0x02;
	UCB1BR1 = 0;

	UCB1CTL0 = UCCKPH + UCMSB + UCMST + UCMODE_0 + UCSYNC;

	UCB1CTL1 &= ~UCSWRST;
	UCB1IFG &= ~UCRXIFG;

	Dogs102x6_writeCommand(Dogs102x6_initMacro, 13);
}

void CMA3000_init(void) {
    P2DIR  &= ~BIT5;	// mode: input
    P2OUT  |=  BIT5;
    P2REN  |=  BIT5;	// enable pull up resistor
    P2IE   |=  BIT5;	// interrupt enable
    P2IES  &= ~BIT5;	// process on interrupt's front
    P2IFG  &= ~BIT5;	// clear interrupt flag

    // set up cma3000 (CBS - Chip Select (active - 0))
    P3DIR  |=  BIT5;	// mode: output
    P3OUT  |=  BIT5;	// disable cma3000 SPI data transfer

    // set up ACCEL_SCK (SCK - Serial Clock)
    P2DIR  |=  BIT7;	// mode: output
    P2SEL  |=  BIT7;	// clk is  UCA0CLK

    // Setup SPI communication
    P3DIR  |= (BIT3 | BIT6);	// Set MOSI and PWM pins to output mode
    P3DIR  &= ~BIT4;		// Set MISO to input mode
    P3SEL  |= (BIT3 | BIT4);	// Set mode : P3.3 - UCA0SIMO , P3.4 - UCA0SOMI
    P3OUT  |= BIT6;		// Power cma3000
    UCA0CTL1 = UCSSEL_2 | UCSWRST;

    UCA0BR0 = 0x30;
    UCA0BR1 = 0x0;

    UCA0CTL0 = UCCKPH & ~UCCKPL | UCMSB | UCMST | UCSYNC | UCMODE_0;

    UCA0CTL1 &= ~UCSWRST;

    // dummy read from REVID
    CMA3000_writeCommand(0x04, NONE);
    __delay_cycles(1250);

    // write to CTRL register
    CMA3000_writeCommand(0x0A, BIT4 | BIT2 | BIT7 );
    __delay_cycles(25000);
}

void start_A1()
{
    TA1CTL |= TACLR;
    TA1CTL |= MC__UP;
    TA1CCTL0 |= CCIE;
}

void stop_A1()
{
    TA1CTL &= ~MC__UP;
    TA1CCTL0 &= ~CCIE;
}

uint8_t CMA3000_writeCommand(uint8_t mosi_byte1, uint8_t mosi_byte2) {

    char miso_data;

        P3OUT &= ~BIT5;
        P2IE &= ~BIT5;

        miso_data = UCA0RXBUF; //FREE RECEIVER BUFF(unknown data) and set UCRXIFG --> 0

        start_A1();

        whatChecking = 1;
        __bis_SR_register(LPM0_bits + GIE);
        UCA0TXBUF = mosi_byte1;

        whatChecking = 2;
        __bis_SR_register(LPM0_bits + GIE);
        miso_data = UCA0RXBUF; //FREE RECEIVER BUFF (readed data - PORST + 010) and set UCRXIFG --> 0

        whatChecking = 1;
        __bis_SR_register(LPM0_bits + GIE);
        UCA0TXBUF = mosi_byte2;

        whatChecking = 2;
        __bis_SR_register(LPM0_bits + GIE);
        miso_data = UCA0RXBUF; //FREE RECEIVER BUFF (data from mosi_byte1 address) and set UCRXIFG --> 0
                               //WHY FOR WRITE OPERATION NEEDED

        whatChecking = 3;
        __bis_SR_register(LPM0_bits + GIE);

        stop_A1();
        P3OUT |= BIT5;
        P2IE |= BIT5;

        return miso_data;

}

void init_A1()
{
    TA1CCR0 = 20000;//5242;//5242 for 0.005 s
    TA1CTL |= TASSEL__SMCLK;
    TA1CTL |= TACLR;

    TA1CTL &= ~MC__UP;
    //TA1CCTL0 |= CCIE;
}

long int parseProjectionByte(uint8_t projectionByte) {
	int i = 0;
	long int projectionValue = 0;

	int isNegative = projectionByte & BIT7;

	for (; i < 7; i++) {
		if (isNegative) {
			projectionValue += (BITx[i] & projectionByte) ? 0 : MAPPING_VALUES[i];
		}
		else {
			projectionValue += (BITx[i] & projectionByte) ? MAPPING_VALUES[i] : 0;
		}
	}

	projectionValue *= isNegative ? -1 : 1;

	return projectionValue;
}

int calculateAngleFromProjection(double projection) {
	projection /= 1000;
	projection = projection > 1 ? 1 : projection < -1 ? -1 : projection;

	double angle = acos(projection);
	angle *= 57.3;

	return (int) angle;
}

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;
	//9?011?200 - DCOCLKDIV
	    UCSCTL1 = DCORSEL_3;
	    UCSCTL3 |= FLLREFDIV__1;
	    UCSCTL2 = 274;
	    UCSCTL2 |= FLLD__1;

	    init_A1();
	P2DIR &= ~BIT2;
	P2OUT |= BIT2;
	P2REN |= BIT2;
	P2IE |= BIT2;
	P2IES |= BIT2;
	P2IFG = 0;

	P1DIR |= BIT5;
	P1OUT &= ~BIT5;

	Dogs102x6_init();
	Dogs102x6_backlightInit();
	Dogs102x6_clearScreen();
	CMA3000_init();

	__bis_SR_register(LPM0_bits + GIE);

	return 0;
}

