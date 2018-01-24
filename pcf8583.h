/*
 * pcf8583.h
 *
 *  Created on: 20-01-2018
 *      Author: Jeff
 */

#ifndef PCF8583_H_
#define PCF8583_H_

#include "main.h"

#define PCF8583_WRITE 0xA0
#define PCF8583_READ 0xA1

uint8_t bcd_to_bin( uint8_t bcd);

uint8_t bin2bcd(uint8_t bin);

void PCF8583_write(uint8_t address, uint8_t data);

uint8_t PCF8583_read(uint8_t address);

void pcf8583_set_time(czas_t *czas);
void pcf8583_read_time(czas_t *czas);


#endif /* PCF8583_H_ */
