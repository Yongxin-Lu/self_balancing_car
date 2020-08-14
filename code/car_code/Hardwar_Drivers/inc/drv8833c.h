/*
 * drv8833c.h
 *
 *  Created on: 2020Äê5ÔÂ28ÈÕ
 *      Author: simonlu
 */
 
#include "stm32f10x.h"
#include "usart.h"

#ifndef DRV8833C_H_
#define DRV8833C_H_

#define COUNT_VALUE 3599     //Freq 20KHz

#define LEFT 0
#define RIGHT 1

#define LEFT_ENCODER 2
#define RIGHT_ENCODER 3

void DRV8833C_Init(void);
void Motor_GPIO_Init(void);
void Motor_PWM_Init(void);
void Motor_Encoder_Init(void);

void Motor_Set(uint8_t side, int16_t speed);
int16_t Read_Encoder(uint8_t TIMX);

#endif /* DRV8833C_H_ */
