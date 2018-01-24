/*
 * eeprom.h
 *
 *  Created on: 21-01-2018
 *      Author: Jeff
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include "avr/eeprom.h"

void eeprom_init();

void eeprom_write(uint16_t addr, uint8_t data);

uint8_t eeprom_read(uint16_t addr);

#endif /* EEPROM_H_ */
