/*
 * led.h
 *
 *  Created on: 2020年5月13日
 *      Author: simonlu
 */

#include "stm32f10x.h"

#ifndef LED_H_
#define LED_H_

#define LED_PORT   GPIOB
#define LED_PIN    GPIO_Pin_2

#define LED_ON() GPIO_ResetBits(LED_PORT,LED_PIN)
#define LED_OFF() GPIO_SetBits(LED_PORT,LED_PIN)

void LED_Init(void);

#endif /* LED_H_ */
