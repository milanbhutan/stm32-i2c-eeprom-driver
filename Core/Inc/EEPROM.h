/*
 * EEPROM.h
 *
 *  Created on: Aug 8, 2025
 *      Author: milan
 */
#include "main.h"

#ifndef INC_EEPROM_H_
#define INC_EEPROM_H_
#define EEPROM_ADDRESS 0x50
#define EEPROM_MEMORY_ADDR 0x1FF6


void EEPROM_init(void);

void EEPROM_write_string(char str[],char addr[]);

void EEPROM_read_string(char str[],char addr[], char num_bytes[]);

#endif /* INC_EEPROM_H_ */
