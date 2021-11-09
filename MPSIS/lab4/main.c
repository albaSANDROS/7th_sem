#include <msp430.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

volatile int display_mode = 0; //0 -NORMAL, 1 - MIRROR


#define SET_COLUMN_ADDRESS_LSB        0x00
#define SET_COLUMN_ADDRESS_MSB        0x10
#define SET_PAGE_ADDRESS              0xB0

unsigned char whatChecking = 0;
unsigned char isRxReady = 0;
unsigned char isTxReady = 0;
unsigned char isBusy = 1;


// Pins from MSP430 connected to LCD
#define CD              BIT6
#define CS              BIT4
#define RST             BIT7
#define BACKLT          BIT6
#define SPI_SIMO        BIT1
#define SPI_CLK         BIT3


#define NONE                        0
#define READ_X_AXIS_DATA            0x18
#define READ_Y_AXIS_DATA            0x1C
#define READ_Z_AXIS_DATA            0x20

double CONVERT = 9.8*100*100/1000/2.54;
int MAPPING_VALUES[] = { 4571, 2286, 1142, 571, 286, 143, 71 };


uint8_t init_cmds[] =
                    {0x40, //установка начальной строки скроллинга = 0 (без скроллинга)
                     0xA1, //зеркальный режим адресации столбцов
                     0xC0, //нормальный режим адресации строк
                     0xA4, //запрет режима включения всех пикселей (на экран отображается содержимое памяти)
                     0xA6, //отключение инверсного режима экрана
                     0xA2, //смещение напряжения делителя 1/9
                     0x2F, //включение питания усилителя, регулятора и повторителя
                     0x27, //установка контраста
                     0x81,
                     0x10,
                     0xFA, //установка температурной компенсации -0.11%/°С
                     0x90,
                     0xAF  //включение экрана
                    };

int f_abs(int a){
    if(a > 0)
        return a;
    return -a;
}



uint8_t distance = 0x00;
uint8_t digit_0[] = {0b11111111, 0b11111111, 0b00000011, 0b00000011, 0b11111111, 0b11111111,
                     0b00011111, 0b00011111, 0b00011000, 0b00011000, 0b00011111, 0b00011111};

uint8_t digit_1[] = {0b00000011, 0b00000011, 0b11111111, 0b11111111, 0b00000011, 0b00000011,
                     0b00000000, 0b00000000, 0b00011111, 0b00011111, 0b00011000, 0b00000000};

uint8_t digit_2[] = {0b11100011, 0b11100011, 0b01100011, 0b01100011, 0b01111111, 0b01111111,
                     0b00011111, 0b00011111, 0b00011000, 0b00011000, 0b00011000, 0b00011000};

uint8_t digit_3[] = {0b11111111, 0b11111111, 0b01100011, 0b01100011, 0b01100011, 0b01100011,
                     0b00011111, 0b00011111, 0b00011000, 0b00011000, 0b00011000, 0b00011000};

uint8_t digit_4[] = {0b11111111, 0b11111111, 0b11000000, 0b11000000, 0b11000000, 0b11000000,
                     0b00011111, 0b00011111, 0b00000001, 0b00000001, 0b00011111, 0b00011111};

uint8_t digit_5[] = {0b01111111, 0b01111111, 0b01100011, 0b01100011, 0b11100011, 0b11100011,
                     0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b00011111, 0b00011111};

uint8_t digit_6[] = {0b01111111, 0b01111111, 0b01100011, 0b01100011, 0b11111111, 0b11111111,
                     0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b00011111, 0b00011111};

uint8_t digit_7[] = {0b11111111, 0b11111111, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
                     0b00011111, 0b00011111, 0b00011000, 0b00011000, 0b00011000, 0b00011000};

uint8_t digit_8[] = {0b11111111, 0b11111111, 0b11100011, 0b11100011, 0b11111111, 0b11111111,
                     0b00011111, 0b00011111, 0b00011000, 0b00011000, 0b00011111, 0b00011111};

uint8_t digit_9[] = {0b11111111, 0b11111111, 0b01100011, 0b01100011, 0b11100011, 0b11100011,
                     0b00011111, 0b00011111, 0b00011000, 0b00011000, 0b00011111, 0b00011111};

uint8_t plus_sign[] = {0b11100000, 0b11100000, 0b11111100, 0b11111100, 0b11100000, 0b11100000,
                       0b00000000, 0b00000000, 0b00000111, 0b00000111, 0b00000000, 0b00000000};

uint8_t minus_sign[] = {0b11100000, 0b11100000, 0b11100000, 0b11100000, 0b11100000, 0b11100000,
                        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000};



void startTimerA1(){
    TA1CCTL0 = CCIE;
    TA1CCR0 = 5240;
    TA1EX0 = TAIDEX_4;
    TA1CTL = TASSEL_2 | ID__4 | MC_1 | TACLR;
}

void stopTimerA1() {
    TA1CCTL0 &= ~CCIE;
    TA1CTL = MC_0;
}


void init_LCD_pins()
{
    P5DIR |= BIT7;   //LCD_RST
    P5SEL &= ~BIT7;

    P5OUT &= ~BIT7;

    __delay_cycles(25000);
    P5OUT |= BIT7;
    __delay_cycles(125000);

    P4DIR |= BIT1; //SIMO data
    P4SEL |= BIT1;

    P4DIR |= BIT3;  //SCLK
    P4SEL |= BIT3;

    P5DIR |= BIT6; // LCD_D/C  0- comand 1- data
    P5SEL &= ~BIT6;

    P7DIR |= BIT4;  // LCD_CS choose device(=0)
    P7SEL &= ~BIT4;
    P7OUT |= BIT4;

    P7DIR |= BIT6; //LCD_BL_EN backlight power
    P7SEL &= ~BIT6;
    P7OUT |= BIT6;

}

void init_USCI()
{
    UCB1CTL1 |= UCSWRST; //enable program reset 0 , 1 - logical

    UCB1CTL0 |= UCMSB; // order of transfer (  0 — LSB, 1- MSB)
    UCB1CTL0 |= UCMST; // mode 0 — Slave, 1 – Master

    UCB1CTL0 |= UCCKPH; // phase of tackt 0 - change on first capture on second, 1 - on the contrary

    UCB1CTL1 |= UCSSEL1; // source for tact impuls SMCLK

    UCB1CTL1 &= ~UCSWRST;  // en program reset
}





void clear_LCD(void)
{
    uint8_t LcdData[] = { 0x00 };
    uint8_t p, c;

    for (p = 0; p < 8; p++)
    {
        int pa = p;
        int ca = 0;
        uint8_t cmd[1];
        cmd[0] = SET_PAGE_ADDRESS + pa;
        uint8_t H = 0b00000000;
        uint8_t L = 0b00000000;
        uint8_t ColumnAddress[] = { SET_COLUMN_ADDRESS_MSB, SET_COLUMN_ADDRESS_LSB };
        // Separate Command Address to low and high
        L = (ca & 0b00001111);
        H = (ca & 0b11110000);
        H = (H >> 4);

        ColumnAddress[0] = SET_COLUMN_ADDRESS_LSB + L;
        ColumnAddress[1] = SET_COLUMN_ADDRESS_MSB + H;


        Dogs102x6_writeCommand(cmd, 1); // set page address

        Dogs102x6_writeCommand(ColumnAddress, 2);// set column address
        for (c = 0; c < 132; c++)
        {
            Dogs102x6_writeData(LcdData, 1);
        }
    }
}

void display_symbol(uint8_t *digit, uint8_t column)
{
    uint8_t page_cmd;
    uint8_t column_cmd[2];
    uint8_t column_LSB;
    uint8_t column_MSB;
    int p;


    for(p=0; p < 2; p++)
    {
        column_LSB = 0b00001111 & column;
        column_MSB = (0b11110000 & column) >> 4;

        page_cmd = 0b10110000 + p; //
        column_cmd[0] = 0b00000000 + column_LSB;
        column_cmd[1] = 0b00010000 + column_MSB;

        Dogs102x6_writeCommand(&page_cmd, 1);
        Dogs102x6_writeCommand(column_cmd, 2);

        Dogs102x6_writeData(&distance, 2);

        Dogs102x6_writeData(digit +  6 * (p), 6);
     }
}




void display_num(long int number)
{
    long divided_num = number;
    int digit;
    int digit_count = 0;



    while(divided_num != 0)
    {
        digit = f_abs(divided_num % 10);

        switch(digit)
        {
            case 0:
                display_symbol(digit_0, digit_count * 8);
                break;
            case 1:
                display_symbol(digit_1, digit_count * 8 );
                break;
            case 2:
                display_symbol(digit_2, digit_count * 8 );
                break;
            case 3:
                display_symbol(digit_3, digit_count * 8 );
                break;
            case 4:
                display_symbol(digit_4, digit_count * 8 );
                break;
            case 5:
                display_symbol(digit_5, digit_count * 8 );
                break;
            case 6:
                display_symbol(digit_6, digit_count * 8 );
                break;
            case 7:
                display_symbol(digit_7, digit_count * 8 );
                break;
            case 8:
                display_symbol(digit_8, digit_count * 8 );
                break;
            case 9:
                display_symbol(digit_9, digit_count * 8 );
                break;
        }

        divided_num = (divided_num / 10);

        digit_count++;
    }

    if(number > 0)
    {
        display_symbol(plus_sign, digit_count * 8 );
    }
    else if(number < 0)
    {
        display_symbol(minus_sign, digit_count * 8);
    }
}

inline long int parseProjectionByte(uint8_t projectionByte) {
    uint8_t BITx[] = { BIT6, BIT5, BIT4, BIT3, BIT2, BIT1, BIT0 };
    int i = 0;
    long int projectionValue = 0;
    int isNegative = projectionByte & BIT7;
    for (; i < 7; i++) {
        if (isNegative) {
            projectionValue += (BITx[i] & projectionByte) ? 0 : MAPPING_VALUES[i];
        }else{
            projectionValue += (BITx[i] & projectionByte) ? MAPPING_VALUES[i] : 0;
        }
    }
    projectionValue *= isNegative ? -1 : 1;
    return projectionValue;
}

#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ACCEL_ISR(void)
{
    volatile uint8_t zProjectionByte = CMA3000_writeCommand(READ_Z_AXIS_DATA, NONE);

    volatile int zAxisProjection = parseProjectionByte(zProjectionByte);
    zAxisProjection += 215;

    volatile long int inchPerSecondSquaredMultiplied = zAxisProjection * CONVERT;

    clear_LCD();
    display_num(zAxisProjection);

    if(-1000 <= zAxisProjection && zAxisProjection <= -866){
        P1OUT |= BIT4;
    }else{
        P1OUT &= ~BIT4;
    }

}

void Dogs102x6_writeData(uint8_t* sData, uint8_t i)
{
    P7OUT &= ~CS;
    P5OUT |= CD;

    while (i)
    {
        while (!(UCB1IFG & UCTXIFG)); // is ts buffer ready to write data
        UCB1TXBUF = *sData;
        sData++;
        i--;
    }


    while (UCB1STAT & UCBUSY);
    UCB1RXBUF;

    P7OUT |= CS;
}

// sCmd - array of commands, i - amount of commands to run
void Dogs102x6_writeCommand(uint8_t* sCmd, uint8_t i)
{
    // set command mode
    P7OUT &= ~CS;
    P5OUT &= ~CD;

    while (i)
    {
        while (!(UCB1IFG & UCTXIFG));

        UCB1TXBUF = *sCmd;

        sCmd++;
        i--;
    }

    // Wait for all TX/RX to finish
    while (UCB1STAT & UCBUSY);
    // Dummy read to empty RX buffer and clear any overrun conditions
    UCB1RXBUF;

    // back to data mode
    P7OUT |= CS;
}

void CMA3000_init(void) {
    P2DIR  &= ~BIT5;    // mode: input
    P2OUT  |=  BIT5;
    P2REN  |=  BIT5;    // enable pull up resistor
    P2IE   |=  BIT5;    // interrupt enable
    P2IES  &= ~BIT5;    // process on interrupt's front
    P2IFG  &= ~BIT5;    // clear interrupt flag
    // set up cma3000 (CBS - Chip Select (active - 0))
    P3DIR  |=  BIT5;    // mode: output
    P3OUT  |=  BIT5;    // disable cma3000 SPI data transfer
    // set up ACCEL_SCK (SCK - Serial Clock)
    P2DIR  |=  BIT7;    // mode: output
    P2SEL  |=  BIT7;    // clk is  UCA0CLK
    // Setup SPI communication
    P3DIR  |= (BIT3 | BIT6);    // Set MOSI and PWM pins to output mode
    P3DIR  &= ~BIT4;        // Set MISO to input mode
    P3SEL  |= (BIT3 | BIT4);    // Set mode : P3.3 - UCA0SIMO , P3.4 - UCA0SOMI
    P3OUT  |= BIT6;     // Power cma3000
    UCA0CTL1 = UCSSEL_2 | UCSWRST;
    UCA0BR0 = 0x30;
    UCA0BR1 = 0x0;
    UCA0CTL0 = UCCKPH & ~UCCKPL | UCMSB | UCMST | UCSYNC | UCMODE_0;
    UCA0CTL1 &= ~UCSWRST;
    // dummy read from REVID
    CMA3000_writeCommand(0x04, NONE);
    __delay_cycles(1250);
    // write to CTRL register
    CMA3000_writeCommand(0x0A, BIT2 | BIT4);
    __delay_cycles(25000);
}
// byte_one - frame part 1 (8-2: address, 1: R/W, 0: always 0)
// byte_two - frame part 2 (data when W or anything when R)
uint8_t CMA3000_writeCommand(uint8_t firstByte, uint8_t secondByte) {
    char miso_data;

    P3OUT &= ~BIT5;
    P2IE &= ~BIT5;

    miso_data = UCA0RXBUF; //FREE RECEIVER BUFF(unknown data) and set UCRXIFG --> 0

    startTimerA1();

    whatChecking = 1;
    __bis_SR_register(LPM0_bits + GIE);
    UCA0TXBUF = firstByte;
    isTxReady = 0;

    whatChecking = 2;
    __bis_SR_register(LPM0_bits + GIE);
    miso_data = UCA0RXBUF; //FREE RECEIVER BUFF (readed data - PORST + 010) and set UCRXIFG --> 0
    isRxReady = 0;

    whatChecking = 1;
    __bis_SR_register(LPM0_bits + GIE);
    UCA0TXBUF = secondByte;
    isTxReady = 0;

    whatChecking = 2;
    __bis_SR_register(LPM0_bits + GIE);
    miso_data = UCA0RXBUF; //FREE RECEIVER BUFF (data from mosi_byte1 address) and set UCRXIFG --> 0
                               //WHY FOR WRITE OPERATION NEEDED
    isRxReady = 0;

    whatChecking = 3;
    __bis_SR_register(LPM0_bits + GIE);
    isBusy = 1;


    stopTimerA1();
    P3OUT |= BIT5;
    P2IE |= BIT5;

    return miso_data;
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void TIMER_1 (void) {
    if(whatChecking == 1)
    {
            if(UCA0IFG & UCTXIFG)
            {
                isTxReady = 1;
                __bic_SR_register_on_exit(LPM0_bits + GIE);
            }
        }
        else if(whatChecking == 2)
        {
            if(UCA0IFG & UCRXIFG)
            {
                isRxReady = 1;
                __bic_SR_register_on_exit(LPM0_bits + GIE);
            }
        }
        else if(whatChecking == 3)
        {
            if(!(UCA0STAT & UCBUSY))
            {
                isBusy = 0;
                __bic_SR_register_on_exit(LPM0_bits + GIE);
            }
        }
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;

    P1DIR |= BIT4; //LED 7
    P1OUT &= ~BIT4;

    init_LCD_pins();

    init_USCI();

    Dogs102x6_writeCommand(init_cmds, 13);


    clear_LCD();
    CMA3000_init();
    __bis_SR_register(LPM0_bits + GIE);
    __no_operation();

    return 0;
}


