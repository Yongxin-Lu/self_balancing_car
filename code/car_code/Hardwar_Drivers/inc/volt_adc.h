/*
 * volt_adc.h
 *
 *  Created on: 2020Äê5ÔÂ28ÈÕ
 *      Author: simonlu
 */
 
#include "stm32f10x.h"

#ifndef VOLT_ADC_H_
#define VOLT_ADC_H_

#define ADC1_DR_ADDRESS ((uint32_t)0x40012400+ 0x4c)

void Volt_ADC_Init(void);
void ADC1_GPIO_Init(void);
void ADC1_Mode_Init(void);

float Get_BattVolt(void);

#endif /* VOLT_ADC_H_ */
