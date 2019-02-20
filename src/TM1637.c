/*----------------------------------------------------------------------------

                          *============*
                          |            |
                          | enterCode  |
                          |            |
                          *============*


 Authors: T. HILAIRE
 Licence: GPL v3

 File: TM1637.c
       low-level driver for the TM1637 chip
       - basic function to driver the TM1637 chip


Copyright 2019 T. Hilaire

----------------------------------------------------------------------------*/


#include <util/delay.h>
#include "TM1637.h"


uint8_t brightness;	/* global brightness */


/*=======================*
 * low level  functions  *
 *=======================*/

/* send the start signal */
void start()
{
	set_CLK();
	set_DIO();
	clear_DIO();
	clear_CLK();
}

/* send the start signal */
void stop()
{
	clear_CLK();
	clear_DIO();
	set_CLK();
	set_DIO();
}

/* send a value to the TM1637 */
uint8_t writeByte(uint8_t value)
{
	uint8_t ack;

	/* send 8-bit data */
	for(uint8_t i = 0; i<8; i++)
	{
		clear_CLK();
		write_DIO(value & 1);
		value >>= 1;
		set_CLK();
	}

	/* wait for ACK */
	clear_CLK();
	set_DIO();
	set_CLK();
	set_DIO_Input();
	_delay_us(50);
	ack = get_DIO();
	if (!ack)
	{
		set_DIO_Output();
		clear_DIO();
	}
	_delay_us(50);
	set_DIO_Output();
	_delay_us(50);

	return ack;
}



/*=================================*
 * high level frond-end functions  *
 *=================================*/


/* clear the TM1637, set the brightness */
void TM1637_setup(uint8_t bright)
{
	brightness = bright;
	start();
	writeByte(0x80 + brightness);
	stop();

	// clear display
	TM1637_write4(0x00, 0x00, 0x00, 0x00);
}


/* display a number at a given position */
void TM1637_write(uint8_t data, uint8_t pos)
{

	start();
	writeByte(ADDR_FIXED);
	stop();

	start();
	writeByte(pos | STARTADDR);
	writeByte(data);
	stop();

	start();
	writeByte(0x88 + brightness);
	stop();
}


/* display 4 numbers */
void TM1637_write4(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth)
{
	start();
	writeByte(ADDR_AUTO);
	stop();

	start();
	writeByte(0xC0);
	writeByte(first);
	writeByte(second);
	writeByte(third);
	writeByte(fourth);
	stop();

	start();
	writeByte(0x88 + brightness);
	stop();

}
