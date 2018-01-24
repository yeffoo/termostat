/*
 * eeprom.c
 *
 *  Created on: 21-01-2018
 *      Author: Jeff
 */

#include "eeprom.h"

void eeprom_init() {
	EECR |= (1 << EEPE);
}

void eeprom_write(uint16_t addr, uint8_t data) {
	while( EECR & (1 << EEPE) );
	EEAR = addr;
	EEDR = data;
	EECR |= (1 << EEMPE);
	EECR |= (1 << EEPE);
}

uint8_t eeprom_read(uint16_t addr) {
	while( EECR & (1 << EEPE) );
	EEAR = addr;
	EECR |= (1 << EERE);
	return EEDR;
}

