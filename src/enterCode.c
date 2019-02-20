/*----------------------------------------------------------------------------

                          *============*
                          |            |
                          | enterCode  |
                          |            |
                          *============*


 Authors: T. HILAIRE
 Licence: GPL v3

 File: enterCode.c
       main function for the enterCode project
       - wait for a key, and turn on/off LEDs accordingly


Copyright 2019 T. Hilaire

----------------------------------------------------------------------------*/


#include <util/delay.h>
#include <avr/io.h>
#include <avr/eeprom.h>

#include "enterCode.h"
#include "TM1637.h"
#include "codes.h"


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





int main()
{
	uint8_t key;							/* key hit on the keyboard */
	uint8_t accessCode[CODE_SIZE + 4];			/* actual code the user is typing on the keyboard */
	uint8_t pos = 4;						/* position of the next char (the 4 char are NO_KEY) */
	uint8_t codes[NB_CODES][CODE_SIZE];		/* array of valid codes (#0 is the master code) */
	uint8_t nC;								/* code number */
	uint8_t status = ACCESS_MODE;

    /* input/output config */
	DDRA = 0b00000011;       /* PA0 and PA1 are output */
    DDRB = 0b11101111;       /* PB4 is input, other are output */
    DDRD = 0b11110000;       /* PD0 to PD4 are input */
    PORTD = 0b00001111;		 /* PD0 to PD4 with pull-up */

    /* import the codes */
	//readCodesEEPROM(codes);
	eeprom_read_block(&(codes[0][0]), (uint8_t*)0, NB_CODES*CODE_SIZE);

    /* init code */
	for(uint8_t i=0; i<CODE_SIZE+4; i++)
		accessCode[i] = NO_KEY;

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
			/* validation of the code */
			if (status == ACCESS_MODE) {
				/* NORMAL MODE */
				nC = checkCode(codes, accessCode);
				/* reset code and display*/
				for (uint8_t i=0; i < CODE_SIZE+4; i++)
					accessCode[i] = NO_KEY;
				TM1637_write4(0, 0, 0, 0);
				pos = 4;
				/* check what to do now */
				if (nC ==0) {
					/* the master code has been entered */
					status = MASTER_MODE;
					pos = 0;
					/* turn on the two LEDs */
					PORTA |= 0b00000011;
				}
				else if (nC != 255) {
					/* green led */
					PORTA |= 0b00000010;
					_delay_ms(2000);
					PORTA &= ~0b00000010;
				} else {
					/* red led */
					PORTA |= 0b00000001;
					_delay_ms(2000);
					PORTA &= ~0b00000001;
				}
			} else {
				/* MASTER MODE */
				/* turn off the two LEDs */
				PORTA &= ~0b00000011;
				/* update the code and write in the EEPROM */
				updateCode(codes, accessCode);
				//writeCodesEEPROM(codes);
				eeprom_update_block(&(codes[0][0]), (uint8_t*)0, NB_CODES*CODE_SIZE);
				/* reset code and display */
				for (uint8_t i=0; i < CODE_SIZE+4; i++)
					accessCode[i] = NO_KEY;
				TM1637_write4(0, 0, 0, 0);
				pos = 4;
				/* green light to tell that it's ok */
				PORTA |= 0b00000010;
				_delay_ms(1000);
				PORTA &= ~0b00000010;
			}
		}
		else if (key == KEY_AST) {
			/* delete the last key */
			if (pos>4)
				pos--;
			accessCode[pos] = NO_KEY;
			TM1637_write4(CODE_CHAR[accessCode[pos-4]], CODE_CHAR[accessCode[pos-3]], CODE_CHAR[accessCode[pos-2]], CODE_CHAR[accessCode[pos-1]]);
		}
		else if ( (key != NO_KEY) && (pos<(4+CODE_SIZE)) ) {
			/* new key */
			TM1637_write4(CODE_CHAR[accessCode[pos-3]], CODE_CHAR[accessCode[pos-2]], CODE_CHAR[accessCode[pos-1]], CODE_CHAR[key]);
			accessCode[pos] = key;
			pos++;
			/* in Master mode, the 1st key is the code number, so we move to the 4th position after that */
			if (pos==1)
				pos = 4;
		}

		_delay_ms(500);
	}

}