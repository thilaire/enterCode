#include <util/delay.h>
#include <avr/io.h>
#include <avr/cpufunc.h>

#include "TM1637.h"

/* input/output
 * PA0: (out) LED1 (red)            PB0: (out) Keyboard 0
 * PA1: (out) LED2 (green)          PB1: (out) Keyboard 1
 * PD0: (in) Keyboard 0             PB2: (out) Keyboard 2
 * PD1: (in) Keyboard 1             PB3: (out) Keyboard 3
 * PD2: (in) Keyboard 2             PB4: (in) RS522 IRQ
 * PD3: (in) Keyboard 3             PB5: (out) SPI MOSI
 * PD4: (out) TM1637 DIO            PB6: (out) SPI MISO
 * PD5: (out) TM1637 CLK            PB7: (out) SPI SCK
 * PD6: (???) RS522
*/

/* maximum code size and number of codes (master + others)*/
#define CODE_SIZE	8
#define NB_CODES	4

/* keys */
#define KEY_SHARP 	14
#define KEY_AST		12
#define NO_KEY		16


/* debug function */
void Green() {
	PORTA |= 0b00000010;
	_delay_ms(100);
	PORTA &= ~0b00000010;
	_delay_ms(100);
}

void Red() {
	PORTA |= 0b00000001;
	_delay_ms(100);
	PORTA &= ~0b00000001;
	_delay_ms(100);
}
void GreenRed() {
	PORTA |= 0b00000011;
	_delay_ms(100);
	PORTA &= ~0b00000011;
	_delay_ms(100);
}

const uint8_t NUMBER_FONT[] = {
		0b00111111, // 0
		0b00000110, // 1
		0b01011011, // 2
		0b01001111, // 3
		0b01100110, // 4
		0b01101101, // 5
		0b01111101, // 6
		0b00000111, // 7
		0b01111111, // 8
		0b01101111, // 9
		0b01110111, // A
		0b01111100, // B
		0b00111001, // C
		0b01011110, // D
		0b01111001, // E
		0b01110001  // F
};


const uint8_t CODE_CHAR[] = {
		0b00000110, // 1
		0b01011011, // 2
		0b01001111, // 3
		0b01110111, // A
		0b01100110, // 4
		0b01101101, // 5
		0b01111101, // 6
		0b01111100, // B
		0b00000111, // 7
		0b01111111, // 8
		0b01101111, // 9
		0b00111001, // C
		0,
		0b00111111, // 0
		0,
		0b01011110, // D
		0
};



void EEPROM_write(unsigned int ucAddress, unsigned char ucData) {
/* Wait for completion of previous write */ while(EECR & (1<<EEPE));
/* Set Programming mode */
	EECR = (0<<EEPM1)|(0<<EEPM0);
/* Set up address and data registers */ EEAR = ucAddress;
	EEDR = ucData;
/* Write logical one to EEMPE */
	EECR |= (1<<EEMPE);
	/* Start eeprom write by setting EEPE */
	EECR |= (1<<EEPE);
}


unsigned char EEPROM_read(unsigned int ucAddress) {
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE));
/* Set up address register */
	EEAR = ucAddress;
/* Start eeprom read by writing EERE */ EECR |= (1<<EERE);
/* Return data from data register */ return EEDR;
}


/* read the codes in the EEPROM */
void readCodesEEPROM(uint8_t codes[NB_CODES][CODE_SIZE]) {

	uint8_t* tab = &(codes[0][0]);
	for(uint8_t i=0; i<NB_CODES*CODE_SIZE; i++) {
		*(tab++) = EEPROM_read(i);
	}

}

void writeCodesEEPROM(uint8_t codes[NB_CODES][CODE_SIZE]) {

	uint8_t* tab = &(codes[0][0]);
	for(uint8_t i=0; i<NB_CODES*CODE_SIZE; i++) {
		EEPROM_write(i, *(tab++));
	}

}






/* check if the code is correct
 * returns 255 if the code is not correct, otherwise return the code number*/
uint8_t checkCode(uint8_t codes[NB_CODES][CODE_SIZE], uint8_t theCode[CODE_SIZE]) {
	/* check individually each code */
	for(uint8_t i=0; i<NB_CODES; i++) {
		uint8_t j=4;
		while ((theCode[j]==codes[i][j-4]) && (theCode[j] != NO_KEY))
			j++;
		if (theCode[j]==codes[i][j-4])
			return i;
	}
	/* otherwise, the code is not equal to one of the codes */
	return 255;
}

/* returns the number of the 1st bit set to 0
 * x MUST BE non null */
uint8_t getZeroBit(uint8_t x)
{
	uint8_t line = 0;
	while (x&1) {
		x >>= 1;
		line++;
	}
	return line;
}



/* main function: wait for a key, and return the key character
 *
 * the NOP after each PORTB changes are *required*
 * */
uint8_t waitForKey(){
	uint8_t line, col;

	/* set the output lines to 0 */
	PORTB &= ~(0b00001111);
	_NOP();
	/* wait for a key and get the line*/
	//TODO: use interrupt !!
	while ((PIND&15) == 15);
	line = getZeroBit(PIND&15);
	/* wait for debounce (useful?) */
	_delay_ms(100);
	/* get column number, by toggling each column */
	PORTB |= 0b00001111;_NOP();
	for(col=0; col<4; col++) {
		PORTB &= ~(1<<col);
		_NOP();_NOP();
		if ((PIND&15) != 15)
			break;
		PORTB |= 0b00001111;
		_NOP();
	}
	if (col==4)
		return NO_KEY;

	/* wait the key is released */
	PORTB &= ~(0b00001111);
	_NOP();
	while((PIND&15) != 15);

	return (line<<2) + col;
}


int main()
{
	uint8_t key;							/* key hit on the keyboard */
	uint8_t code[CODE_SIZE + 4];			/* actual code the user is typing on the keyboard */
	uint8_t pos = 4;						/* position of the next char (the 4 char are NO_KEY) */
	uint8_t codes[NB_CODES][CODE_SIZE];		/* array of valid codes (#0 is the master code) */
	uint8_t nC;								/* code number */


    /* input/output config */
	DDRA = 0b00000011;       /* PA0 and PA1 are output */
    DDRB = 0b11101111;       /* PB4 is input, other are output */
    DDRD = 0b11110000;       /* PD0 to PD4 are input */
    PORTD = 0b00001111;		 /* PD0 to PD4 with pull-up */

    /* import the codes */
	readCodesEEPROM(codes);

    /* init code */
	for(uint8_t i=0; i<CODE_SIZE+4; i++)
		code[i] = NO_KEY;

    /* init message */
    TM1637_setup(5);
    TM1637_write4(0b00111001, 0b00111111, 0b01011110, 0b01111001);	/* COdE */
	_delay_ms(3000);
	TM1637_setup(5);

	/* main loop */
	while(1){
		/* wait */
		key = waitForKey();
		/* manage the key */
		if (key == KEY_SHARP) {
			/* end of the code */
			nC = checkCode(codes, code);
			if (nC != 255) {
				for(uint8_t i=0; i<nC; i++) {
					Green();
					_delay_ms(500);
				}
				/* green led */
				PORTA |= 0b00000010;
				_delay_ms(2000);
				PORTA &= ~0b00000010;
			}
			else {
				/* red led */
				PORTA |= 0b00000001;
				_delay_ms(2000);
				PORTA &= ~0b00000001;
			}
			/* reset code */
			for(uint8_t i=0; i<CODE_SIZE+4; i++)
				code[i] = NO_KEY;
			TM1637_write4(0,0,0,0);
			pos = 4;
		}
		else if (key == KEY_AST) {
			/* delete the last key */
			if (pos>4)
				pos--;
			code[pos] = NO_KEY;
			TM1637_write4(CODE_CHAR[code[pos-4]], CODE_CHAR[code[pos-3]], CODE_CHAR[code[pos-2]], CODE_CHAR[code[pos-1]]);
		}
		else if ( (key != NO_KEY) && (pos<(4+CODE_SIZE)) ) {
			/* new key */
			TM1637_write4(CODE_CHAR[code[pos-3]], CODE_CHAR[code[pos-2]], CODE_CHAR[code[pos-1]], CODE_CHAR[key]);
			code[pos] = key;
			pos++;
		}

		_delay_ms(500);
	}

}