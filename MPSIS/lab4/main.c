#include <msp430.h>
#include<stdio.h>

typedef unsigned char byte;
typedef unsigned short word;

#define SYMB_COUNT 12

#define SYMB_LENGTH 7
#define SYMB_WIDTH 4
#define GAP 4

#define LEFT_LIMIT -9999
#define RIGHT_LIMIT +9999

#define DATA 0
#define COMMAND 1

#define MAX_DIGIT_COUNT 4
#define MAX_SYMB_COUNT 5

#define START_POINT_X 90
#define START_POINT_Y 5

#define SCROLL_STEP 8

#define PAGE_WIDTH 8

//шаблоны цифр и знаков
byte templates[SYMB_COUNT][SYMB_LENGTH] = {
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

//текущее число для вывода на экран
int currentNumber = 8659;
//копия текущего числа для вывода на экран(в процессе вывода оно будет редактироваться)
int tempCurrentNumber = 0;
//слагаемое для изменения числа по нажатию кнопки
const int jump = -921;
//значение текущего скроллинга
int curScroll = 0;


//настройка кнопок
void setupButtons() {
	//P1 BIT7 - S1
	P1OUT |= BIT7;
	P1REN |= BIT7;
	P1IFG &= ~BIT7;
	P1IES |= BIT7;
	P1IE |= BIT7;

	//S2
	P2OUT |= BIT2;
	P2REN |= BIT2;
	P2IES |= BIT2;
	P2IFG &= ~BIT2;
	P2IES |= BIT2;
	P2IE |= BIT2;
}

//запись данных либо команд в контроллер
void writeToController(byte* info, int length, int actionType) {
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
void setAddress(byte page, byte column) {
	//работаем с областью памяти, попадающей на дисплей
	if (column > 101) {
		column = 100;
	}
	// if(page>7){
	// 	page=7;
	// }
	//формируем адрес текущей страницы
	byte selectPageCom[1];
	selectPageCom[0] = 0xB0 + page;

	//формируем адрес текущего столбца
	byte selectColCom[2];
	byte msb = 0x10 + ((column & 0xF0) >> 4);
	byte lsb = 0x00 + (column & 0x0F);
	selectColCom[0] = lsb;
	selectColCom[1] = msb;

	writeToController(selectPageCom, 1, COMMAND);
	writeToController(selectColCom, 2, COMMAND);
}
//очистка дисплея
void clearScreen() {
	byte page;
	byte column;
	byte zero[1];
	zero[0]=0x00;

	for (page=0; page < 8; page++) {
		setAddress(page, 0);
		for (column=0; column < 132; column++) {
			writeToController(zero, 1, DATA);
		}
	}
}

//подсчёт разрядов в числе
int calcDigits() {
	int count = 1;
	int temp = currentNumber;
	while (temp/=10) {
		++count;
	}
	return count;
}

//возведение в степень
int pow(int a, int b){
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
	int delitel = pow(10,decimalPos-1);//делитель для извлечения текущего разряда
	int digit = tempCurrentNumber / delitel;//текущий разряд
	tempCurrentNumber = tempCurrentNumber - delitel*digit;//убираем текущий разряд из числа
	return digit;
}
//вывод числа на дисплей
void printNumber() {
	tempCurrentNumber = currentNumber>0?currentNumber:currentNumber*(-1);
	//считаем количество разрядов и символов(учитывается знак)
	byte digitCount = calcDigits();
	byte symbCount = digitCount+1;

	//находим первую задействованную страницу
	int y = START_POINT_Y + curScroll * SCROLL_STEP - 1;
	byte firstUsedPage = 0;
	while(y/PAGE_WIDTH){
		y-=PAGE_WIDTH;
		++firstUsedPage;
	}
	//находим последнюю задействованную страницу
	y = START_POINT_Y +  curScroll * SCROLL_STEP + symbCount*SYMB_WIDTH + (symbCount-1)*GAP-1;
	byte lastUsedPage = 0;
	while(y/PAGE_WIDTH){
		y-=PAGE_WIDTH;
		++lastUsedPage;
	}
	//начинаем с первой задействованной страницы
	byte curPage=firstUsedPage;

	byte zero[1];
	zero[0]=0x00;

	byte curColumn = 0;
	byte curDigit = 0;
	byte curSymb[SYMB_LENGTH];


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
				byte columnVal[1];
				columnVal[0]=curSymb[innerIndx];
				++innerIndx;
				writeToController(columnVal, 1, DATA);
			}
			//остальная облать заполняется нулями
			else
				writeToController(zero, 1, DATA);

		}
		//переходим к следующему разряду
		++curDigit;
	}
}
//настройка дисплея и SPI
void initialization(byte* commands) {
	P5DIR |= BIT7;// конфигурируем для работы с дисплеем
	//устанавливаем сигнал сброса 
	P5OUT &= ~BIT7;
	P5OUT |= BIT7;

	//выбираем ведомого(начинаем его настройку)
	P7DIR |= BIT4;
	P7OUT &= ~BIT4;

	//устанавливаем режим команд
	P5DIR |= BIT6;//необходимая настройка
	P5OUT &= ~BIT6;//непосредственно устанавливаем режим команд

	P4SEL |= BIT1; // передача данных LCD_SIMO
	P4DIR |= BIT1;

	P4SEL |= BIT3; // синхросигнал SCLK
	P4DIR |= BIT3;

	// настраиваем питание подсветки
	P7DIR |= BIT6;
	P7OUT |= BIT6;
	P7SEL &= ~BIT6;

	//заканчиваем настройку ведомого
	P7OUT |= BIT4;


	UCB1CTL1 |= UCSWRST;//настройка регистров управления SPI всегда начинается с сброса	
	UCB1CTL0 = UCCKPH + UCMSB + UCMST + UCMODE_0 + UCSYNC; // фаза Ти(захват по первому перепаду изменение по второму) , первым следует старший бит (старший (полу-)байт записывается первым), master mode, 3-pin SPI, синхронный режим
	//UCB1CTL0 &= ~UCCKPL;
	UCB1CTL1 = UCSSEL__SMCLK + UCSWRST;// источником тактирования для ucb используем smclk, всегда устанавливаем состояние "сброс" при программном конфигурировании
	

	UCB1BR0 = 0x02; // младший байт делителя частоты,  "UCBxBR0 + UCBxBR1 * 256 = формирует значение делителя частоты UCBRx"
	UCB1BR1 = 0; //  старший байт делителя частоты

	UCB1CTL1 &= ~UCSWRST; // снимаем "блокировку сбросом", устанавливая ноль
	UCB1IFG &= ~UCRXIFG; //снимаем флаг прерывания (который устанавливается при получении входным сдвиг. регистром данных)
	//устанавливаем "начальную конфигурацию дисплея"
	writeToController(commands, 13, COMMAND);
}



#pragma vector=PORT1_VECTOR // сложение
__interrupt void INT1() {
	P1IE &= ~BIT7;
	volatile unsigned int time = 1500;
	while (time) { --time; }

	if (!(P1IN & BIT7)) {
		// Изменяем значение числа в соответствии с вариантом
		currentNumber += jump;
		if (currentNumber < LEFT_LIMIT) {
			currentNumber = RIGHT_LIMIT + (currentNumber - LEFT_LIMIT);
		}
		clearScreen();
		printNumber();
	}
	
	P1IFG &= ~BIT7;
	P1IE |= BIT7;
}


//текущий "уровень" скроллинга
byte scrollingLevel=1;

#pragma vector=PORT2_VECTOR // скроллинг
__interrupt void INT2() {
	P2IE &= ~BIT2;
	volatile unsigned int time = 2500;
	while (time) { --time; }

	if (!(P2IN & BIT2)) {

		byte scrollCom[1];
		byte curScroll=0;
		//в зависимомти от текущего уровня скроллинга выбираем само значение для скроллинга
		switch(scrollingLevel){
		case 0:
			curScroll = 0;
			break;
		case 1:
			curScroll = 57;
			break;
		case 2:
			curScroll = 49;
			break;

		}
		//формируем команду для записи в контроллер дисплея
		scrollCom[0]=0x40+curScroll;
		//устанавливаем значение скроллинга в контроллер дисплея
		writeToController(scrollCom, 1, COMMAND);
		clearScreen();
		printNumber();

		//меняем текущий уровень скроллинга
		if(scrollingLevel==2)
			scrollingLevel=0;
		else
			++scrollingLevel;
	}

	P2IFG &= ~BIT2;
	P2IE |= BIT2;
}



int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	setupButtons();

	// Список команд для инициализации дисплея
	byte controllerInitData[] = {
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

	// Непосредственно инициализация дисплея, его очистка и вывод "дефолтного" числа
	initialization(controllerInitData);
	clearScreen();
	printNumber();

	__bis_SR_register(GIE);
	__no_operation();

	return 0;
}


