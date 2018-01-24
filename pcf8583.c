/*
 * pcf8583.c
 *
 *  Created on: 20-01-2018
 *      Author: Jeff
 */

#include "twi.h"
#include "pcf8583.h"

uint8_t bcd_to_bin( uint8_t bcd) {
	return ( 10*(bcd >> 4) + (bcd & 0x0f) );
}

uint8_t bin2bcd(uint8_t bin) {
    return ( ( (bin / 10) << 4) | (bin % 10) );
}

void PCF8583_write(uint8_t address, uint8_t data) {
	twi_start();
	twi_write(PCF8583_WRITE);
	twi_write(address);
	twi_write(data);
    twi_stop();
}

uint8_t PCF8583_read(uint8_t address) {
	uint8_t data;
	twi_start();
	twi_write(PCF8583_WRITE);
	twi_write(address);
	twi_start();
	twi_write(PCF8583_READ);
	data = twi_read(NACK);
    twi_stop();
    return data;
}

void pcf8583_set_time(czas_t *czas) {
	PCF8583_write(4, czas->godz);
	PCF8583_write(3, czas->min);
}

void pcf8583_read_time(czas_t *czas) {
	czas->godz = bcd_to_bin(PCF8583_read(4));
	czas->min = bcd_to_bin(PCF8583_read(3));
}

