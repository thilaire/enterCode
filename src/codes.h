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

#ifndef ENTERCODE_CODES_H
#define ENTERCODE_CODES_H


#include "enterCode.h"


/* check if the code is correct
 * returns 255 if the code is not correct, otherwise return the code number*/
uint8_t checkCode(uint8_t codes[NB_CODES][CODE_SIZE], uint8_t accessCode[CODE_SIZE]);


/* wait for a key, and return the key character */
uint8_t waitForKey();

/* update a code */
void updateCode(uint8_t codes[NB_CODES][CODE_SIZE], uint8_t accessCode[CODE_SIZE]);


#endif //ENTERCODE_CODES_H
