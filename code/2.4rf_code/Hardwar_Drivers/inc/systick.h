/*
 * systick.h
 *
 *  Created on: 2020年5月13日
 *      Author: simonlu
 */
#include "stm32f10x.h"

#ifndef SYSTICK_H_
#define SYSTICK_H_

void Delay_ms(volatile uint32_t nTime);
void Delay_us(volatile uint32_t nTime);

#endif /* SYSTICK_H_ */
