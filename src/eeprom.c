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


/* write a byte in EEPROM, taken from the datasheet */
void EEPROM_write(unsigned int ucAddress, unsigned char ucData) {
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE));
	/* Set Programming mode */
	EECR = (0<<EEPM1)|(0<<EEPM0);
	/* Set up address and data registers */
	EEAR = ucAddress;
	EEDR = ucData;
	/* Write logical one to EEMPE */
	EECR |= (1<<EEMPE);
	/* Start EEPROM write by setting EEPE */
	EECR |= (1<<EEPE);
}

/* read a byte in EEPROM, taken from the datasheet */
unsigned char EEPROM_read(unsigned int ucAddress) {
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE));
	/* Set up address register */
	EEAR = ucAddress;
	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);
	/* Return data from data register */
	return EEDR;
}


/* read the codes from the EEPROM */
void readCodesEEPROM(uint8_t codes[NB_CODES][CODE_SIZE]) {
	uint8_t* tab = &(codes[0][0]);
	for(uint8_t i=0; i<NB_CODES*CODE_SIZE; i++) {
		*(tab++) = EEPROM_read(i);
	}
}

/* write the codes into the EEPROM */
void writeCodesEEPROM(uint8_t codes[NB_CODES][CODE_SIZE]) {
	uint8_t* tab = &(codes[0][0]);
	for(uint8_t i=0; i<NB_CODES*CODE_SIZE; i++) {
		EEPROM_write(i, *(tab++));
	}
}

