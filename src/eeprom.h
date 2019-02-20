/*----------------------------------------------------------------------------

                          *============*
                          |            |
                          | enterCode  |
                          |            |
                          *============*


 Authors: T. HILAIRE
 Licence: GPL v3

 File: eeprom.c
       simple function to get/set codes into/from the EEPROM


Copyright 2019 T. Hilaire

----------------------------------------------------------------------------*/


#ifndef ENTERCODE_EEPROM_H
#define ENTERCODE_EEPROM_H


/* read the codes from the EEPROM */
void readCodesEEPROM(uint8_t codes[NB_CODES][CODE_SIZE]);

/* write the codes into the EEPROM */
void writeCodesEEPROM(uint8_t codes[NB_CODES][CODE_SIZE]);


#endif //ENTERCODE_EEPROM_H
