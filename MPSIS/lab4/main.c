#include <msp430.h>
#include <math.h>

#define START_COLUMN 30
#define START_PAGE 6

unsigned char font[12][8] =
{
		{0x00, 0x80, 0x80, 0x00, 0x7F, 0x80, 0x80, 0x7F}, //0
		{0x80, 0x80, 0x80, 0x80, 0x20, 0x40, 0xFF, 0x00}, //1
		{0x80, 0x80, 0x80, 0x80, 0x43, 0x84, 0x88, 0x70}, //2
		{0x00, 0x80, 0x80, 0x00, 0x49, 0x88, 0x88, 0x77}, //3
		{0x00, 0x00, 0x00, 0x80, 0xF8, 0x08, 0x08, 0xFF}, //4
		{0x80, 0x80, 0x80, 0x00, 0xF8, 0x88, 0x88, 0x87}, //5
		{0x80, 0x80, 0x80, 0x80, 0xFF, 0x88, 0x88, 0x8F}, //6
		{0x80, 0x00, 0x00, 0x00, 0x81, 0x86, 0x98, 0xE0}, //7
		{0x00, 0x80, 0x80, 0x00, 0x77, 0x88, 0x88, 0x77}, //8
		{0x80, 0x80, 0x80, 0x00, 0x70, 0x88, 0x88, 0x7F}, //9
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1C, 0x08}, //+
		{0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08}  //-
};

void sendCommand(unsigned char byte)
{
	P5OUT &= ~BIT6;
	while (!(UCB1IFG & UCTXIFG));
	P7OUT &= ~BIT4;
	UCB1TXBUF = byte;
	while (UCB1STAT & UCBUSY);
	P7OUT |= BIT4;
}

void sendData(unsigned char byte)
{
	P5OUT |= BIT6;
	while (!(UCB1IFG & UCTXIFG));
	P7OUT &= ~BIT4;
	UCB1TXBUF = byte;
	while (UCB1STAT & UCBUSY);
	P7OUT |= BIT4;
}

void setPos(char column, char page)
{
	char low = column & 0xF;
	char high = column >> 4;
	sendCommand(low);
	sendCommand(0x10 | high);

	page &= 0xF;
	sendCommand(0xB0 | page);
}

void putSymbol(char column, char page, int symbol)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		setPos(column + i, page);
		sendData(font[symbol][i]);
		setPos(column + i, page + 1);
		sendData(font[symbol][i + 4]);
	}
}

void putNumber(char column, char page, int num)
{
	int minus = 0;
	if (num < 0)
	{
		minus = 1;
		num *= -1;
	}

	int i = 0;
	int newnum;

	do
	{
		newnum = num / 10;
		int num_to_draw = num % 10;
		putSymbol(column - i*6, page, num_to_draw);
		i++;
		num = newnum;
	} while (num != 0);

	if (minus == 0)
	{
		putSymbol(column - i*6, page, 10);
	}
	else
	{
		putSymbol(column - i*6, page, 11);
		num *= -1;
	}
}

void clearDisplay(void)
{
	int i, j;
	for (i = 0; i < 132; i++)
	{
		for (j = 0; j < 8; j++)
		{
			setPos(i, j);
			sendData(0x00);
		}
	}
}

void fastClearDisplay(void)
{
	int i, j;
	for (i = 0; i < START_COLUMN + 7; i++)
	{
		for (j = 0; j < START_PAGE + 2; j++)
		{
			setPos(i, j);
			sendData(0x00);
		}
	}
}

void fastClearDisplayACC(void)
{
	int i, j;
	for (i = 0; i < START_COLUMN + 80; i++)
	{
		for (j = START_PAGE - 4; j <= START_PAGE + 1; j++)
		{
			setPos(i, j);
			sendData(0x00);
		}
	}
}

void LCDsetup(void)
{
	//LCD startup config
	P7SEL &= ~BIT6;
	P7SEL &= ~BIT4;
	P7DIR |= BIT6 | BIT4;
	P7OUT |= BIT6 | BIT4;
	P4DIR |= BIT1 | BIT3;
	P4SEL |= BIT3 | BIT1;
	P5SEL &= ~BIT6;
	P5DIR |= BIT6;

	    //RESET LCD
	    P5SEL &= ~BIT7;
	    P5DIR |= BIT7;
	    P5OUT |= BIT7;

	    //stop reseting
	    P5OUT &= ~BIT7;

	    P5OUT &= ~BIT7;
	    __delay_cycles(25000);

	    P5OUT |= BIT7;
	    __delay_cycles(125000);

	    //SPI startup config
	    UCB1CTL1 |= UCSWRST;
	    UCB1CTL0 |= UCCKPH | UCMSB | UCMST | UCSYNC;
	    UCB1CTL1 |= UCSSEL__SMCLK;
	    UCB1BR0	= 0x01;
	    UCB1BR1	= 0;
	    UCB1CTL1 &= ~UCSWRST;

	    sendCommand(0x2F); // power
	    sendCommand(0xAF); // turn on display
	    sendCommand(0xA6); // show display memory
	    sendCommand(0xA1); // set inverted columns idk :/

	    clearDisplay();
}

#define READ_X			0x18  //read from address 06h
#define READ_Y 			0x1C  //read from address 07h
#define READ_Z			0x20  //read from address 08h

struct ang{
	double x;
	double y;
	double z;
}angle;

unsigned char setAc(unsigned char byte_one, unsigned char byte_two)
{
    char indata;

    P3OUT &= ~BIT5;		// enable cma3000 SPI data transfer

    indata = UCA0RXBUF;
    while(!(UCA0IFG & UCTXIFG));

    UCA0TXBUF = byte_one;

    while(!(UCA0IFG & UCRXIFG));

    indata = UCA0RXBUF;

    while(!(UCA0IFG & UCTXIFG));

    UCA0TXBUF = byte_two;

    while(!(UCA0IFG & UCRXIFG));

    indata = UCA0RXBUF;

    while(UCA0STAT & UCBUSY);

    P3OUT |= BIT5;	// disable cma3000 SPI data transfer

    return indata;
}

void SetupAccelerometer()
{
    // INT signal
    P2DIR  &= ~BIT5;	// mode: input
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
    P3DIR  &= ~BIT4;			// Set MISO to input mode
    P3SEL  |= (BIT3 | BIT4);	// Set mode : P3.3 - UCA0SIMO , P3.4 - UCA0SOMI
    P3OUT  |= BIT6;				// Power cma3000

    UCA0CTL1 |= UCSWRST;		// set UCSWRST bit to disable USCI and change its control registers

    UCA0CTL0 = (
		UCCKPH 	&	// UCCKPH - 1: change out on second signal change, capture input on first one)
		~UCCKPL |	// UCCKPL - 0: active level is 1
		UCMSB 	|	// MSB comes first, LSB is next
		UCMST 	|	// Master mode
		UCSYNC 	|	// Synchronous mode
		UCMODE_0	// 3 pin SPI mode
	);

	// set SMCLK as source and keep RESET
	UCA0CTL1 = UCSSEL_2 | UCSWRST;

	// set frequency divider
	UCA0BR0 = 0x30;	// LSB to 48
	UCA0BR1 = 0x0;	// MSB to 0

	UCA0CTL1 &= ~UCSWRST;	// enable USCI

	// dummy read from REVID
	setAc(0x04, 0);
	__delay_cycles(550);

	// write to CTRL register
	setAc(
		0x0A,
		BIT4 |	// -I2C
		BIT1	// 100Hz
	);
	__delay_cycles(10500);
}

long int get_mili_g(unsigned char byte)
{
	unsigned char sign = byte & BIT7;
	unsigned char bits[] = { BIT6, BIT5, BIT4, BIT3, BIT2, BIT1, BIT0 };
	long int mapping[] = { 4571, 2286, 1141, 571, 286, 143, 71 };

	int i = 0;
	long int projection = 0;
	for (; i < 7; i++)
	{
		if (!sign)
		{
			projection += (bits[i] & byte) ? mapping[i] : 0;
		}
		else
		{
			projection += (bits[i] & byte) ? 0 : mapping[i];
		}
	}

	projection = sign ? projection * (-1) : projection;

	return projection;
}

void get_angle(double projection_x, double projection_y, double projection_z)
{
	projection_x = (projection_x > 1000) ? 1000 : (projection_x < -1000 ? -1000 : projection_x);
	projection_y = (projection_y > 1000) ? 1000 : (projection_y < -1000 ? -1000 : projection_y);
	projection_z = (projection_z > 1000) ? 1000 : (projection_z < -1000 ? -1000 : projection_z);

	double projection_x2 = projection_x * projection_x;
	double projection_y2 = projection_y * projection_y;
	double projection_z2 = projection_z * projection_z;

	double ax0 = atan(projection_x/sqrt(projection_y2 + projection_z2)) * 57.3;
	double ax1 = atan(projection_x/projection_z)* 57.3;
	double ax2 = asin(projection_x/1000)* 57.3;

	double ay0 = atan(projection_y/sqrt(projection_x2 + projection_z2)) * 57.3;
	double ay1 = atan(projection_y/projection_z)* 57.3;;
	double ay2 = asin(projection_y/1000)* 57.3;

	double az0 = atan(projection_z/sqrt(projection_x2 + projection_y2)) * 57.3;
	//double az1 = 0;
	//double az2 = 0;

	angle.x = projection_z < 0 ? ax0 : projection_x > 0 ? 180 - ax0 : -180 - ax0;
	angle.y = projection_z < 0 ? ay0 : projection_y > 0 ? 180 - ay0 : -180 - ay0;;
	angle.z = az0+90;
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    //LED 6 config
    P1DIR |= BIT3;
    P1OUT &= ~BIT3;

    LCDsetup();
    SetupAccelerometer();

    __bis_SR_register(LPM3_bits | GIE);

    __no_operation();

	return 0;
}

#pragma vector = PORT2_VECTOR
__interrupt void __Accelerometer_ISR(void)
{
	__delay_cycles(1000);

	volatile unsigned char x_projection_byte = setAc(READ_X, 0);
	__delay_cycles(550);

	volatile unsigned char y_projection_byte = setAc(READ_Y, 0);
	__delay_cycles(550);

	volatile unsigned char z_projection_byte = setAc(READ_Z, 0);
    __delay_cycles(550);

	volatile long int x_projection = get_mili_g(x_projection_byte)/1.212 - 117;
	volatile long int y_projection = get_mili_g(y_projection_byte)/1.141 - 125;
	volatile long int z_projection = get_mili_g(z_projection_byte)/1.071;

	get_angle(x_projection, y_projection, (-1)*z_projection);

	float coeff = 0.981;

	fastClearDisplayACC();
	putNumber(START_COLUMN, START_PAGE, x_projection * coeff);
	putNumber(START_COLUMN, START_PAGE-2, y_projection * coeff);
	putNumber(START_COLUMN, START_PAGE-4, z_projection * coeff);
	putNumber(START_COLUMN+50, START_PAGE, (int)(angle.x));
	putNumber(START_COLUMN+50, START_PAGE-2, (int)(angle.y));
	putNumber(START_COLUMN+50, START_PAGE-4, (int)(angle.z));

	if ((angle.y >= 30) && (angle.y <= 150))
	{
		P1OUT |= BIT3;
	}
	else
	{
		P1OUT &= ~BIT3;
	}
}
