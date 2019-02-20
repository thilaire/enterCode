/*----------------------------------------------------------------------------

                          *============*
                          |            |
                          | enterCode  |
                          |            |
                          *============*


 Authors: T. HILAIRE
 Licence: GPL v3

 File: TM1637.h
       low-level driver for the TM1637 chip
       - basic function to driver the TM1637 chip


Copyright 2019 T. Hilaire

----------------------------------------------------------------------------*/


#ifndef _TM1637_H_
#define _TM1637_H_

#include <avr/io.h>

#define BIT(x) (0x01 << (x))

/* Constants to define the Ports for DIO (Data IO) and the CLK (Clock) */
#define TM1637_DIO_PORT      PORTD
#define TM1637_DIO_DDR       DDRD
#define TM1637_DIO_PINR		 PIND
#define TM1637_DIO_PIN       4

#define TM1637_CLK_PORT      PORTD
#define TM1637_CLK_DDR       DDRD
#define TM1637_CLK_PIN       5


/* intern bit manipulation macros */
inline __attribute__((always_inline)) static void set_CLK() { TM1637_CLK_PORT |= BIT(TM1637_CLK_PIN); }
inline __attribute__((always_inline)) static void clear_CLK() { TM1637_CLK_PORT &= ~BIT(TM1637_CLK_PIN); }

inline __attribute__((always_inline)) static void set_DIO_Input() {TM1637_DIO_DDR &= ~(BIT(TM1637_DIO_PIN));}
inline __attribute__((always_inline)) static uint8_t get_DIO() {return TM1637_DIO_PINR & BIT(TM1637_DIO_PIN);}

inline __attribute__((always_inline)) static void set_DIO_Output() {TM1637_DIO_DDR |= BIT(TM1637_DIO_PIN);}
inline __attribute__((always_inline)) static void set_DIO() {TM1637_DIO_PORT |= BIT(TM1637_DIO_PIN);}
inline __attribute__((always_inline)) static void clear_DIO() {TM1637_DIO_PORT &= ~(BIT(TM1637_DIO_PIN));}
inline __attribute__((always_inline)) static void write_DIO(uint8_t b) { b ? set_DIO() : clear_DIO(); }




/* some constants (cf TM1637 manual)*/
#define ADDR_AUTO  0x40
#define ADDR_FIXED 0x44
#define STARTADDR  0xc0


/* initialize the display and set the brightness */
void TM1637_setup(uint8_t brightness);

/* display 4 numbers */
void TM1637_write4(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth);

/* display a number at a given position */
void TM1637_write(uint8_t ch, uint8_t pos);

#endif