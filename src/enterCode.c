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

/* code size: MUST BE a power of 2*/
#define CODE_SIZE	8

/* keys */
#define KEY_SHARP 	14
#define KEY_AST		12
#define NO_KEY		16


void Green() {
	PORTA |= 0b00000010;
	_delay_ms(100);
	PORTA &= ~0b00000010;
}

void Red() {
	PORTA |= 0b00000001;
	_delay_ms(100);
	PORTA &= ~0b00000001;
}
void GreenRed() {
	PORTA |= 0b00000011;
	_delay_ms(100);
	PORTA &= ~0b00000011;
}



/* debug */
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


/* check if the code is correct */
uint8_t checkCode(uint8_t* code, uint8_t pos) {
	if ( (code[pos]==0) && (code[(pos-1) & (CODE_SIZE-1)]==1) && (code[(pos-2) & (CODE_SIZE-1)]==2) )
		return 1;
	else
		return 0;
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



/* main function: wait for a key, and return the key character */
uint8_t waitForKey(){
	uint8_t line, col;

	/* set the output lines to 0 */
	PORTB &= ~(0b00001111);
	_NOP();
	/* wait for a key */
	//TODO: use interrupt !!
	while ((PIND&15) == 15);	/* attente touche */
	line = getZeroBit(PIND&15);

	_delay_ms(100);

	/* get column number */
	PORTB |= 0b00001111;_NOP();
	for(col=0; col<4; col++) {
		PORTB &= ~(1<<col);
		_NOP();_NOP();
		if ((PIND&15) != 15)		/* TODO: use ~ */
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


uint8_t waitForKeyToRemove(){
	PORTB &= ~(0b00001111);
	_NOP();
	TM1637_write(NUMBER_FONT[PIND&15],0);

	PORTB |= 0b00001111;
	_NOP();
	TM1637_write(NUMBER_FONT[PIND&15],2);
}




int main()
{
	uint8_t key;
	uint8_t code[CODE_SIZE];
	uint8_t pos = 0;

    /* input/output config */
	DDRA = 0b00000011;       /* PA0 and PA1 are output */
    DDRB = 0b11101111;       /* PB4 is input, other are output */
    DDRD = 0b11110000;       /* PD0 to PD4 are input */
    PORTD = 0b00001111;		 /* PD0 to PD4 with pull-up */

    /* init code */
	for(uint8_t i=0; i<CODE_SIZE; i++)
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
			if (checkCode(code, pos-1)) {
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
			for(uint8_t i=0; i<CODE_SIZE; i++)
				code[i] = 16;
			TM1637_write4(0,0,0,0);
		}
		else if (key == KEY_AST) {
			/* delete the last key */
			pos = (pos-1) & (CODE_SIZE-1);
			code[pos] = NO_KEY;
			TM1637_write4(CODE_CHAR[code[(pos-4) & (CODE_SIZE-1)]], CODE_CHAR[code[(pos-3) & (CODE_SIZE-1)]], CODE_CHAR[code[(pos-2) & (CODE_SIZE-1)]], CODE_CHAR[code[(pos-1) & (CODE_SIZE-1)]]);
		}
		else if (key != NO_KEY) {
			/* new key */
			TM1637_write4(CODE_CHAR[code[(pos-3) & (CODE_SIZE-1)]], CODE_CHAR[code[(pos-2) & (CODE_SIZE-1)]], CODE_CHAR[code[(pos-1) & (CODE_SIZE-1)]], CODE_CHAR[key]);
			code[pos] = key;
			pos = (pos+1) & (CODE_SIZE-1);
		}

		_delay_ms(500);
	}

}