#include <msp430.h>
#include<stdio.h>
#include<math.h>
#include<stdint.h>

#define SYMB_COUNT 12
#define SYMB_LENGTH 7
#define SYMB_WIDTH 4
#define GAP 4


#define DATA 0
#define COMMAND 1

#define MAX_DIGIT_COUNT 4
#define MAX_SYMB_COUNT 5

#define START_POINT_X 90
#define START_POINT_Y 5

#define PAGE_WIDTH 8

#define PADDING 0
#define Z_AXIS_VALUE 0x08
#define Y_AXIS_VALUE 0x07
#define REVID 0x01
#define CTRL 0x02
#define READ 0x00
#define WRITE 0x02

#define CHECK_TX 1
#define CHECK_RX 2
#define CHECK_BUSY 3
int checkItem = 0;

#define LOWER_VALUE -1125 //-866
#define TOP_VALUE 1125 //866

//шаблоны цифр и знаков
uint8_t templates[SYMB_COUNT][SYMB_LENGTH] = {
        {0x0F, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0F}, //0
        {0x0E, 0x04, 0x04, 0x04, 0x04, 0x07, 0x06}, //1
        {0x0F, 0x01, 0x01, 0x06, 0x08, 0x08, 0x0F}, //2
        {0x0F, 0x08, 0x08, 0x0E, 0x08, 0x08, 0x0F}, //3
        {0x08, 0x08, 0x08, 0x0F, 0x09, 0x09, 0x09}, //4
        {0x0F, 0x08, 0x08, 0x0F, 0x01, 0x01, 0x0F}, //5
        {0x0F, 0x09, 0x09, 0x0F, 0x01, 0x01, 0x0F}, //6
        {0x01, 0x01, 0x02, 0x04, 0x08, 0x08, 0x0F}, //7
        {0x06, 0x09, 0x09, 0x06, 0x09, 0x09, 0x06}, //8
        {0x0F, 0x08, 0x08, 0x0F, 0x09, 0x09, 0x0F}, //9
        {0x00, 0x06, 0x06, 0x0F, 0x06, 0x06, 0x00}, //+
        {0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00}  //-
};


int currentNumber = 0;
int tempCurrentNumber = 0;

void startTimer()
{
    TA1CTL |= MC__UP | TACLR;
    TA1CCTL0 |= CCIE;
}

void stopTimer()
{
    TA1CTL &= ~MC__UP;
    TA1CCTL0 &= ~CCIE;
}

void timerInitialization(){

	 UCSCTL1 = DCORSEL_3;
	 UCSCTL3 |= FLLREFDIV__1;
	 UCSCTL2 = 274;

	 TA1CCR0 = 40000;
	 TA1CTL |= TASSEL__SMCLK | ID__4 | MC__UP |TACLR;
	 TA1CCTL0 |= CCIE;
}

//запись данных либо команд в контроллер
void writeToScreenController(uint8_t* info, int length, int actionType) {
    P7OUT &= ~BIT4; // выбор ведомого
    if (actionType == DATA) {
        P5OUT |= BIT6;  // выбираем перессылку данных
    }
    else if (actionType == COMMAND) {
        P5OUT &= ~BIT6;  // выбираем перессылку команд
    }

    int i = 0;
    for (i = 0; i < length; i++) {
        while (!(UCB1IFG & UCTXIFG)) {} // ждём флага освобождения сдвигового регистра (данные из сдвигового регистра передаются ведомому по линии UCxSIMO)
        UCB1TXBUF = *(info + i); // записываем данные в буффер (сдвиговый регистр), одновременно сбрасывается UCTXIFG
    }

    while (UCB1STAT & UCBUSY) {}// пока UCBUSY установлен происходит передача данных

    //заканчиваем работу с ведомым
    P7OUT |= BIT4;
}

//установка текущего адреса для записи
void setAddress(uint8_t page, uint8_t column) {
    //работаем с областью памяти, попадающей на дисплей
    if (column > 101) {
        column = 100;
    }

    //формируем адрес текущей страницы
    uint8_t selectPageCom[1];
    selectPageCom[0] = 0xB0 + page;

    //формируем адрес текущего столбца
    uint8_t selectColCom[2];
    uint8_t msb = 0x10 + ((column & 0xF0) >> 4);
    uint8_t lsb = 0x00 + (column & 0x0F);
    selectColCom[0] = lsb;
    selectColCom[1] = msb;

    writeToScreenController(selectPageCom, 1, COMMAND);
    writeToScreenController(selectColCom, 2, COMMAND);
}

//очистка дисплея
void clearScreen() {
    uint8_t page;
    uint8_t column;
    uint8_t zero[1];
    zero[0]=0x00;

    for (page=0; page < 8; page++) {
        setAddress(page, 0);
        for (column=0; column < 132; column++) {
            writeToScreenController(zero, 1, DATA);
        }
    }
}

//подсчёт разрядов в числе
int calcDigits() {
    int count = 1;
    int temp = currentNumber;
    while (temp/=10){
        ++count;
    }
    return count;
}

//возведение в степень
int mypow(int a, int b){
    if(b==0){
        return 1;
    }
    else{
        int temp=a;
        int i=0;
        for(;i<b-1;i++){
            temp*=a;
        }
        return temp;
    }
}

//извлечение разряда из числа
int extractDigit(int currentDigitPos, int symbCount){
    int decimalPos = symbCount - currentDigitPos;//позиция текущего разряда
    int delitel = mypow(10,decimalPos-1);//делитель для извлечения текущего разряда
    int digit = tempCurrentNumber / delitel;//текущий разряд
    tempCurrentNumber = tempCurrentNumber - delitel*digit;//убираем текущий разряд из числа
    return digit;
}
//вывод числа на дисплей
void printNumber() {
    tempCurrentNumber = currentNumber>0?currentNumber:currentNumber*(-1);
    //считаем количество разрядов и символов(учитывается знак)
    uint8_t digitCount = calcDigits();
    uint8_t symbCount = digitCount+1;

    //находим первую задействованную страницу
    int y = START_POINT_Y - 1;
    uint8_t firstUsedPage = 0;
    while(y/PAGE_WIDTH){
        y-=PAGE_WIDTH;
        ++firstUsedPage;
    }
    //находим последнюю задействованную страницу
    y = START_POINT_Y + symbCount*SYMB_WIDTH + (symbCount-1)*GAP-1;
    uint8_t lastUsedPage = 0;
    while(y/PAGE_WIDTH){
        y-=PAGE_WIDTH;
        ++lastUsedPage;
    }
    //начинаем с первой задействованной страницы
    uint8_t curPage=firstUsedPage;

    uint8_t zero[1];
    zero[0]=0x00;

    uint8_t curColumn = 0;
    uint8_t curDigit = 0;
    uint8_t curSymb[SYMB_LENGTH];


    for(;curPage<lastUsedPage;curPage++){
        setAddress(curPage,0);
        int innerIndx = 0;
        //если текущий символ знак
        if(curDigit == 0){
            //если -
            if(currentNumber<0){
                int i=0;
                //получаем "столбцы" нашего знака
                for(;i<SYMB_LENGTH;i++){
                    curSymb[i]=templates[11][i];//-
                }
            }
            //если +
            else{
                int i=0;
                //получаем "столбцы" нашего знака
                for(;i<SYMB_LENGTH;i++){
                    curSymb[i]=templates[10][i];//+
                }
            }

        }
        //если текущий символ цифра
        else{
            //извлекаем цифру из числа
            int digit = extractDigit(curDigit,symbCount);
            int i=0;
            //получаем "столбцы" нашей цифры
            for(;i<SYMB_LENGTH;i++){
                curSymb[i]=templates[digit][i];//digit
            }
        }
        //сдвигаем на четыре разряда, чтобы записать разряд в нижнюю половину страницы
        int i=0;
        for(;i<SYMB_LENGTH;i++){
            curSymb[i]<<=4;
        }

        //заполняем дисплей информацией
        for(curColumn=0;curColumn<132;curColumn++){
            //если попали в область, занимаемую нашим числом
            if(curColumn > START_POINT_X && curColumn < START_POINT_X + SYMB_LENGTH + 1){
                uint8_t columnVal[1];
                columnVal[0]=curSymb[innerIndx];
                ++innerIndx;
                writeToScreenController(columnVal, 1, DATA);
            }
            //остальная облать заполняется нулями
            else
                writeToScreenController(zero, 1, DATA);

        }
        //переходим к следующему разряду
        ++curDigit;
    }
}

void screenInitialization(uint8_t* commands) {
	P5DIR |= BIT7;// конфигурируем для работы с дисплеем
		P5SEL &= ~BIT7;
		//устанавливаем сигнал сброса
		P5OUT &= ~BIT7;
		__delay_cycles(25000);
		P5OUT |= BIT7;
		__delay_cycles(125000);

		// передача данных LCD_SIMO
		P4SEL |= BIT1;
		P4DIR |= BIT1;
		// синхросигнал SCLK
		P4SEL |= BIT3;
		P4DIR |= BIT3;

		//устанавливаем режим команд
		P5DIR |= BIT6;//необходимая настройка
		P5SEL &= ~BIT6;

		//P5OUT &= ~BIT6;//непосредственно устанавливаем режим команд

		//выбираем ведомого(начинаем его настройку)
		P7SEL &= ~BIT4;
		P7DIR |= BIT4;
		//P7OUT &= ~BIT4;
		P7OUT |= BIT4;

		// настраиваем питание подсветки
		P7DIR |= BIT6;
		P7OUT |= BIT6;
		P7SEL &= ~BIT6;

    // настраиваем spi
    UCB1CTL1 |= UCSWRST;//настройка регистров управления SPI всегда начинается с сброса
    UCB1CTL0 = UCCKPH + UCMSB + UCMST + UCMODE_0 + UCSYNC; // фаза Ти(захват по первому перепаду изменение по второму) , первым следует старший бит (старший (полу-)байт записывается первым), master mode, 3-pin SPI, синхронный режим
    //UCB1CTL0 &= ~UCCKPL;
    UCB1CTL1 = UCSSEL__SMCLK + UCSWRST;// источником тактирования для ucb используем smclk, всегда устанавливаем состояние "сброс" при программном конфигурировании


    UCB1BR0 = 0x01; // младший байт делителя частоты,  "UCBxBR0 + UCBxBR1 * 256 = формирует значение делителя частоты UCBRx"
    UCB1BR1 = 0; //  старший байт делителя частоты

    UCB1CTL1 &= ~UCSWRST; // снимаем "блокировку сбросом", устанавливая ноль
    UCB1IFG &= ~UCRXIFG; //снимаем флаг прерывания (который устанавливается при получении входным сдвиг. регистром данных)
    //устанавливаем "начальную конфигурацию дисплея"
    writeToScreenController(commands, 13, COMMAND);
}

uint8_t accel_rw(uint8_t address, uint8_t data, uint8_t actionType) {

    uint8_t result;

    address <<= 2;
    if(actionType == WRITE){
        address |= WRITE;
    }

    //выбираем (задействуем) акселерометр
    P3OUT &= ~BIT5;
    P2IE &= ~BIT5;

    result = UCA0RXBUF; // считываем буфер RX, чтобы просто очистить флаг прерывания

    startTimer();

    // ждём освобождение сдвигового регистра
    checkItem = CHECK_TX;
    __bis_SR_register(LPM0_bits + GIE);
    UCA0TXBUF = address; //записываем в буфер адрес

    // ждём пока данные запишутся в буффер RX
    checkItem = CHECK_RX;
    __bis_SR_register(LPM0_bits + GIE);
    result = UCA0RXBUF; // считываем буфер RX, чтобы просто очистить флаг прерывания


    checkItem = CHECK_TX;
    __bis_SR_register(LPM0_bits + GIE);
    UCA0TXBUF = data; // записываем фиктивные данные в буффер TX (считаются такими в случае чтения),
                      //в случае записи это должны быть данные для записи в регистр

    // ждём пока данные запишутся в буффер RX
    checkItem = CHECK_RX;
    __bis_SR_register(LPM0_bits + GIE);
    result = UCA0RXBUF; // читаем результат из буффера RX

    // ждём завершения работы
    checkItem = CHECK_BUSY;
    __bis_SR_register(LPM0_bits + GIE);

    stopTimer();

    // заканчиваем работу(deselect) с акселерометр
    P3OUT |= BIT5;
    P2IE |= BIT5;

    return result;
}

void accelInitialization() {
    P2DIR  &= ~BIT5;    // на вход
    P2REN  |=  BIT5;    // задействуем pull up резистор
    P2IE   |=  BIT5;    // разрешаем прерывания
    P2IES  &= ~BIT5;    // прерывания по фронту
    P2IFG  &= ~BIT5;    // очищаем флаг прерывания

    // "отключаем" (unselect) акселерометр
    P3DIR  |=  BIT5;    // mode: output
    P3OUT  |=  BIT5;    // disable cma3000 SPI data transfer

    // выбираем clk для акселерометра
    P2DIR  |=  BIT7;
    P2SEL  |=  BIT7; // UCA0CLK

    // настраиваем spi
    P3DIR  |= (BIT3 | BIT6); // устанавливаем линию передачи данных по SPI: SIMO, а также PWM
    P3DIR  &= ~BIT4;         // устанавливаем линию приёма данных по SPI: SOMI
    P3SEL  |= (BIT3 | BIT4); // режим : Port3.3 - UCA0SIMO , Port3.4 - UCA0SOMI
    P3OUT  |= BIT6;          // подаём питание на акселерометр

    UCA0CTL1 = UCSSEL_2 | UCSWRST;

    UCA0CTL0 = UCCKPH | UCMSB | UCMST | UCSYNC | UCMODE_0;// фаза Ти(захват по первому перепаду изменение по второму), полярность,
                                                          // первым следует старший бит (старший (полу-)байт записывается первым), master mode, 3-pin SPI, синхронный режим

    // настраиваем делитель частоты
    UCA0BR0 = 0x30;
    UCA0BR1 = 0x0;

    UCA0CTL1 &= ~UCSWRST;

    // Чтение регистра REVID
    uint8_t RevID = accel_rw(REVID, PADDING, READ);
    __delay_cycles(1000);

    // Записываем данные в регистр управления акселерометра
    accel_rw(CTRL, BIT7 | BIT4 | BIT2, WRITE); //BIT2 - установка 400 Гц, BIT4 - запрещаем I2C, BIT 7 - 2g
    __delay_cycles(1000);
}


#pragma vector = TIMER1_A0_VECTOR
__interrupt void TIMER_ISR (void)
{
	switch(checkItem){
		case CHECK_TX:{
			if(UCA0IFG & UCTXIFG){
				__bic_SR_register_on_exit(LPM0_bits + GIE);
			}
			break;
		}
		case CHECK_RX:{
			if(UCA0IFG & UCRXIFG){
				__bic_SR_register_on_exit(LPM0_bits + GIE);
			}
			break;
		}
		case CHECK_BUSY:{
			if(!(UCA0STAT & UCBUSY)){
				__bic_SR_register_on_exit(LPM0_bits + GIE);
			}
			break;
		}
	}
}

long get_angle(long projection)
{
	double precised_projection = projection;

	// from mili g to g
	double ratio = precised_projection / 1000;

	ratio = ratio > 1 ? 1 : ratio < -1 ? -1 : ratio;

	volatile double angle = acos(ratio);

	// convert rad to deg
	angle *= 57.3;

	return (long)angle;
}

long getRealValue(uint8_t value){
    // извлекаем знак
    uint8_t sign = value & 0x80;

    // представление "весов" битов регистра из акселерометра
    short templates[] = {1142, 571, 286, 143, 71, 36, 18};
    uint8_t i = 0;
    uint8_t selector = 0x40;
    long result = 0;
    // "собираем" значение проекции из полученных от акселерометра данных
    for(;i<7;i++){
        if(sign){
            result += (selector & value) ? 0 : templates[i];
        }else{
            result += (selector & value) ? templates[i] : 0;
        }
        selector >>= 1;
    }
    if(sign){
        result *= (-1);
    }
    return result;
}


#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ACCEL_ISR(void)
{
    if((P2IN & BIT5))
    {
        int8_t accel_y = 0;
        int8_t accel_z = 0;
        long real_y = 0;
        long real_z = 0;

        // читаем данные о проекциях
        accel_y = accel_rw(Y_AXIS_VALUE, PADDING, READ);
        accel_z = accel_rw(Z_AXIS_VALUE, PADDING, READ);
        __delay_cycles(250);

        // переводим данные из формы выдаваемой акселерометром в "человеческую" форму
        real_y = getRealValue(accel_y);
        real_z = getRealValue(accel_z);

        currentNumber = (real_y * 10);
        clearScreen();
        printNumber();
	    
	long angle = get_angle(real_y);
	angle = real_z > 0 ? angle : angle * (-1); 

        
        // знак определяем по знаку проекции на ось Z (т.е используется плоскость OyOz)
        if((angle >= 0) && (angle <= 180)){
            P1OUT |= BIT5;
        }
        else{
            P1OUT &= ~BIT5;
        }
    }
}



int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    // настраиваем диод для индикации
    P1DIR |= BIT5; // diod 8
    P1OUT &= BIT5; //

    // Список команд для инициализации дисплея
    uint8_t screenInitData[] = {
            0x40,//начальная строка скроллинга (0)
            0xA1,//зеркальный режим адресации столбцов
            0xC0,//нормальный режим адресации строк
            0xA4,//отображение памяти на экран
            0xA6,//отключение инверсного режима экрана
            0xA2,//смещение напряжения делителя 1/9
            0x2F,//включение питания усилителя, регулятора, повторителя
            0x27,//три команды установки контраста
            0x81,
            0x0F,
            0xFA,//две команды установки температурной компенсации -0.11%/Celsium
            0x90,
            0xAF,//включение экрана
    };

    timerInitialization();

    // Непосредственно инициализация дисплея, его очистка и вывод "дефолтного" числа
    screenInitialization(screenInitData);
    clearScreen();
    printNumber();
    accelInitialization();

    __bis_SR_register(LPM0_bits + GIE);
    __no_operation();
    return 0;
}


