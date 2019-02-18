#include <util/delay.h>
#include <avr/io.h>

#include "TM1637.h"

/* input/output
 * PA0: (out) LED1                  PB0: (in) Keyboard 0
 * PA1: (out) LED2                  PB1: (in) Keyboard 1
 * PD0: (out) Keyboard 0            PB2: (in) Keyboard 2
 * PD1: (out) Keyboard 1            PB3: (in) Keyboard 3
 * PD2: (out) Keyboard 2            PB4: (in) RS522 IRQ
 * PD3: (out) Keyboard 3            PB5: (out) SPI MOSI
 * PD4: (out) TM1637 DIO            PB6: (out) SPI MISO
 * PD5: (out) TM1637 CLK            PB7: (out) SPI SCK
 * PD6: (???) RS522
*/


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



int main()
{
    /* input/output config */
	DDRA = 0b00000011;       /* PA0 and PA1 are output */
    DDRB = 0b11100000;       /* PB0 to PB4 are output, PB5 to PB7 are output */
    DDRD = 0b11111111;       /* PD0 to PD7 are output */


    /* test */
    TM1637_setup(0);
    TM1637_write4(NUMBER_FONT[7],NUMBER_FONT[3],NUMBER_FONT[6],NUMBER_FONT[1]);

	//TM1637_write(NUMBER_FONT[7],0);
	//TM1637_write(NUMBER_FONT[1],1);
    //TM1637_write(NUMBER_FONT[3],2);
	//TM1637_write(NUMBER_FONT[6],3);


	while(1){
		PORTA = 0b00000001;
		_delay_ms(500);
		PORTA = 0b00000010;
		_delay_ms(500);	
	}

}