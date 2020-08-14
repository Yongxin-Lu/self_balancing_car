/*
 * led.h
 *
 *  Created on: 2020年5月13日
 *      Author: simonlu
 */

#include "stm32f10x.h"

#ifndef LED_H_
#define LED_H_

#define LED_ON() GPIO_ResetBits(GPIOA,GPIO_Pin_15)
#define LED_OFF() GPIO_SetBits(GPIOA,GPIO_Pin_15)

void LED_Init(void);

#endif /* LED_H_ */
