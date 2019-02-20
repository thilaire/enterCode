/*----------------------------------------------------------------------------

                          *============*
                          |            |
                          | enterCode  |
                          |            |
                          *============*


 Authors: T. HILAIRE
 Licence: GPL v3

 File: codes.c
       some functions to deal with the codes (check, change)


Copyright 2019 T. Hilaire

----------------------------------------------------------------------------*/


#include <util/delay.h>
#include <avr/io.h>
#include <avr/cpufunc.h>

#include "codes.h"


/* check if the code is correct
 * returns 255 if the code is not correct, otherwise return the code number*/
uint8_t checkCode(uint8_t codes[NB_CODES][CODE_SIZE], uint8_t accessCode[CODE_SIZE]) {
	/* check individually each code */
	for(uint8_t i=0; i<NB_CODES; i++) {
		uint8_t j=4;
		while ((accessCode[j]==codes[i][j-4]) && (accessCode[j] != NO_KEY))
			j++;
		if (accessCode[j]==codes[i][j-4])
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
 * the NOP after each PORTB changes is *required*
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

/* update a code */
void updateCode(uint8_t codes[NB_CODES][CODE_SIZE], uint8_t accessCode[CODE_SIZE]) {
	uint8_t iC;	/* index of the code to be updated */

	/* get the number of the code to change (first byte at pos 0 is the key of the code) */
	iC = (accessCode[0]==KEY_ZERO) ? 0 : (accessCode[0] - ((accessCode[0]>>2)&3) + 1);

	/* change the code, if possible */
	if (iC < NB_CODES) {
		for (uint8_t i = 0; i < CODE_SIZE; i++)
			codes[iC][i] = accessCode[i + 4];
	}
}
