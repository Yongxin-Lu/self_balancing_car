/*
 * usart.h
 *
 *  Created on: 2020年5月15日
 *      Author: simonlu
*/

#include "stm32f10x.h"
#include <stdio.h>

#ifndef USART_H_
#define USART_H_

#define USART_BANDRATE 115200

void USART1_Init(void);
void USART1_GPIO_Init(void);
void USART1_SendByte(uint8_t data);
void USART1_SendStr(uint8_t *data);
int fputc(int ch, FILE *f);

#endif /* USART_H_ */
