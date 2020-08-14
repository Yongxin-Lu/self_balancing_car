/*
 * stm32f10x_it.c

 *
 *  Created on: 2020年5月13日
 *      Author: simonlu
 */

#include "stm32f10x_it.h"
#include "buzzer.h"

extern volatile uint32_t TimingDelay;
extern volatile int16_t leftCount;
extern volatile int16_t rightCount;
volatile uint16_t countT=0; //1k times interrupt is 50ms

void SysTick_Handler(void)
{
	if(TimingDelay!=0x00){
		TimingDelay--;
	}
}

void TIM4_IRQHandler(void)
{
	if(TIM4->SR&0X0001)//溢出中断
	{    				   				     	 
		if(countT<200)
		{
			countT++;
		}else{
			countT=0;
			leftCount=TIM2->CNT;
			TIM2->CNT=0;
			rightCount=TIM3->CNT;
			TIM3->CNT=0;
		}
	}				   
	TIM4->SR&=~(1<<0);//清除中断标志位 	
}

void EXTI9_5_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line8)!= RESET)  
  {  
    EXTI_ClearITPendingBit(EXTI_Line8);

  }   
}
