/*
 * systick.c
 *
 *  Created on: 2020年5月13日
 *      Author: simonlu
 */
#include "systick.h"

volatile uint32_t TimingDelay;

/*
   SystemCoreClock/1000 1ms
   SystemCoreClock/100000 10us
   SystemCoreClock/1000000 1us
*/

void Delay_ms(volatile uint32_t nTime)
{
	if(SysTick_Config(SystemCoreClock/1000))
	{
		//Capture Error;
		while(1);
	}
	//Close SysTick;
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;

	TimingDelay=nTime;

	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;

	while(TimingDelay!=0);

	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
}

void Delay_us(volatile uint32_t nTime)
{
	if(SysTick_Config(SystemCoreClock/1000000))
	{
		//Capture Error;
		while(1);
	}
	//Close SysTick;
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;

	TimingDelay=nTime;

	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;

	while(TimingDelay!=0);

	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
}
