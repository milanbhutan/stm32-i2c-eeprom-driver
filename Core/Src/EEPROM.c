#include "main.h"
#include "EEPROM.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void EEPROM_init(void){
	// Configure I2C
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOBEN);
    GPIOB->MODER &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9);
	GPIOB->MODER |=  (GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1);
	GPIOB->OTYPER|= (GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9);
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9);
	GPIOB->PUPDR |= (GPIO_PUPDR_PUPD8_0 | GPIO_PUPDR_PUPD9_0);
	GPIOB->OSPEEDR |=  (( GPIO_OSPEEDR_OSPEED8_1) | (GPIO_OSPEEDR_OSPEED9_1));
	// clear nibble for bit 8 AF (SCL)
	GPIOB->AFR[1] &= ~((0x000F << GPIO_AFRL_AFSEL0_Pos));
	// set b8 AF to SPI1 (fcn 4)
	GPIOB->AFR[1] |=  ((0x0004 << GPIO_AFRL_AFSEL0_Pos));
	// clear nibble for bit 9 AF (SDA)
	GPIOB->AFR[1] &= ~((0x000F << GPIO_AFRL_AFSEL1_Pos));
	// set b9 AF to SPI1 (fcn 4)
	GPIOB->AFR[1] |=  ((0x0004 << GPIO_AFRL_AFSEL1_Pos));
	// enable I2C bus clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;
	// put I2C into reset (release SDA, SCL)
	I2C1->CR1 &= ~( I2C_CR1_PE );
	// filters: enable analog
	I2C1->CR1 &= ~( I2C_CR1_ANFOFF );
	// filters: disable digital
	I2C1->CR1 &= ~( I2C_CR1_DNF );
	// 16 MHz SYSCLK timing from CubeMX
	I2C1->TIMINGR = 0x00220203;
	// auto send STOP after transmission
	I2C1->CR2 |= ( I2C_CR2_AUTOEND );
	 // 7-bit address mode
	I2C1->CR2 &= ~( I2C_CR2_ADD10 );
	 // enable I2C
	I2C1->CR1 |= ( I2C_CR1_PE );
	I2C1->CR1 &= ~( I2C_CR1_PE );
	I2C1->CR1 |= ( I2C_CR1_PE );





}


void EEPROM_write_string(char str[],char addr[]){
	uint16_t mem_addr=(uint16_t)strtol(addr, NULL, 16);
	I2C1->ICR |= I2C_ICR_STOPCF;
	// set WRITE mode
	I2C1->CR2 &= ~( I2C_CR2_RD_WRN );
	// clear Byte count
	I2C1->CR2 &= ~( I2C_CR2_NBYTES );
	// write N=string_length bytes (2 addr, N bytes of data)
	I2C1->CR2 |= ( (strlen(str)+2) << I2C_CR2_NBYTES_Pos);
	// clear device address
	I2C1->CR2 &= ~( I2C_CR2_SADD );
	// device addr SHL 1
	I2C1->CR2 |= ( EEPROM_ADDRESS << (I2C_CR2_SADD_Pos+1) );
	// start I2C WRITE op
	I2C1->CR2 |= I2C_CR2_START;
	// wait for start condition to transmit
	while(!(I2C1->ISR & I2C_ISR_TXIS)) ;
	// xmit MSByte of address
	I2C1->TXDR = (mem_addr >> 8);

	while(!(I2C1->ISR & I2C_ISR_TXIS)) ;
	// xmit LSByte of address
    I2C1->TXDR = mem_addr;

    for(uint32_t i=0;i<strlen(str);i++){
    	// xmit characters
    	while(!(I2C1->ISR & I2C_ISR_TXIS)) ;
    	I2C1->TXDR = (str[i]);

    }


}

void EEPROM_read_string(char str[],char addr[],char num_bytes[]){
	//convert address from string to hexadecimal value
	uint16_t mem_addr=(uint16_t)strtol(addr, NULL, 16);
	//convert num_bytes from string to integer value
	uint16_t num_byt=atoi(num_bytes);
	I2C1->ICR |= I2C_ICR_STOPCF;
	// set WRITE mode
	I2C1->CR2 &= ~( I2C_CR2_RD_WRN );
	// clear Byte count
	I2C1->CR2 &= ~( I2C_CR2_NBYTES );
	// write 2 bytes (2 addr)
	I2C1->CR2 |= ( 2 << I2C_CR2_NBYTES_Pos);
	// clear device address
	I2C1->CR2 &= ~( I2C_CR2_SADD );
	// device addr SHL 1
	I2C1->CR2 |= ( EEPROM_ADDRESS << (I2C_CR2_SADD_Pos+1) );
	// start I2C WRITE op
	I2C1->CR2 |= I2C_CR2_START;
	// wait for start condition to transmit
	while(!(I2C1->ISR & I2C_ISR_TXIS)) ;
	// xmit MSByte of address
	I2C1->TXDR = (mem_addr >> 8);

	while(!(I2C1->ISR & I2C_ISR_TXIS)) ;
	// xmit LSByte of address
    I2C1->TXDR = mem_addr;


	I2C1->ICR |= I2C_ICR_STOPCF;
	// set READ mode
	I2C1->CR2 |= ( I2C_CR2_RD_WRN );
	// clear Byte count
	I2C1->CR2 &= ~( I2C_CR2_NBYTES );
	// read N bytes (N data)
	I2C1->CR2 |= ( num_byt << I2C_CR2_NBYTES_Pos);
	// clear device address
	I2C1->CR2 &= ~( I2C_CR2_SADD );
	// device addr SHL 1
	I2C1->CR2 |= ( EEPROM_ADDRESS << (I2C_CR2_SADD_Pos+1) );
	// start I2C WRITE op
	I2C1->CR2 |= I2C_CR2_START;

	for(uint32_t i=0;i<num_byt;i++){
	while(!(I2C1->ISR & I2C_ISR_RXNE));
	// read consecutive characters from given address
	str[i]=I2C1->RXDR;
	}

}


