/*
 * adc.h
 *
 *  Created on: 2020Äê5ÔÂ28ÈÕ
 *      Author: simonlu
 */
 
#include "stm32f10x.h"

#ifndef ADC_H_
#define ADC_H_

#define ADC1_DR_ADDRESS ((uint32_t)0x40012400+ 0x4c)

void myADC_Init(void);
void ADC1_GPIO_Init(void);
void ADC1_Mode_Init(void);

float Get_BattVolt(void);
uint16_t Get_JoystickX(void);
uint16_t Get_JoystickY(void);

#endif /* ADC_H_ */
