/*
 * w25q128.h
 *
 *  Created on: 2020Äê7ÔÂ15ÈÕ
 *      Author: simonlu
 */

#ifndef __W25Q128_H
#define __W25Q128_H 

#include "stm32f10x.h"

#define MANUFACTURERID 0XEF
#define DEVICEID 0X17

#define FLASH_CHIP_PORT GPIOC
#define FLASH_CHIP_PIN GPIO_Pin_13

#define FLASH_CSN_LOW() GPIO_ResetBits(FLASH_CHIP_PORT,FLASH_CHIP_PIN);
#define FLASH_CSN_HIGH() GPIO_SetBits(FLASH_CHIP_PORT,FLASH_CHIP_PIN);

void W25Q128_Init(void);
void FLASH_GPIO_Init(void);
void FLASH_SPI_Init(void);
uint8_t SPI1_WriteReadByte(uint8_t data);
uint8_t FLASH_ID_Check(void);

#endif /* W25Q128_H_ */
