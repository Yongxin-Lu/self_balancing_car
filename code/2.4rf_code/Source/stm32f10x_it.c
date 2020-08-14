/*
 * stm32f10x_it.c

 *
 *  Created on: 2020年5月13日
 *      Author: simonlu
 */

#include "stm32f10x_it.h"

extern volatile uint32_t TimingDelay;

void SysTick_Handler(void)
{
	if(TimingDelay!=0x00){
		TimingDelay--;
	}
}

void EXTI4_IRQHandler(void)
{
	//LED_OFF();
	EXTI_ClearITPendingBit(EXTI_Line4);
}

void EXTI15_10_IRQHandler(void)
{
	//LED_OFF();
	EXTI_ClearITPendingBit(EXTI_Line11);
	EXTI_ClearITPendingBit(EXTI_Line15);
}

void EXTI3_IRQHandler(void)
{
	//LED_ON();
	EXTI_ClearITPendingBit(EXTI_Line3);
}
