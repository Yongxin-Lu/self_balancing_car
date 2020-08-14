/*
 * drv8833c.c
 *
 *  Created on: 2020年5月28日
 *      Author: simonlu
 *   Attention: MicroLib must disabled,otherwise pwm polarity of TIM1 will be reversed.
 *      Edited: 08/12, add input speed value limit function in Motor_Set().
 */
 
#include "drv8833c.h"

/***********************************************************************************
 *
 * Right motor speed is higher than left, e.g. set right 60% and left 100% to let 
 * two motors have same speed.
 *
 **********************************************************************************/

volatile int16_t leftCount=0;
volatile int16_t rightCount=0;

/***********************************************************************************/
void DRV8833C_Init(void)
{
	Motor_GPIO_Init();
	Motor_PWM_Init();
  Motor_Encoder_Init();              
}
/***********************************************************************************/
void Motor_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;      //原来的pwm引脚与飞线pa10相连，设置高阻来断开
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;     //Motor 1 Control, Left
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;   //Motor 1 Encoder Pin Init
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;     //Motor 2 Control, Right
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;   //Motor 2 Encoder Pin Init
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}
/***********************************************************************************/
void Motor_PWM_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//TIM 4
	TIM_TimeBaseStructure.TIM_Period=COUNT_VALUE;
	TIM_TimeBaseStructure.TIM_Prescaler=0;
	TIM_TimeBaseStructure.TIM_ClockDivision= TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse=0;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OC3Init(TIM4,&TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Enable);
	
	TIM_OC4Init(TIM4,&TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM4,ENABLE);
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM4,ENABLE);
	//TIM1
	TIM_TimeBaseStructure.TIM_Period=COUNT_VALUE;
	TIM_TimeBaseStructure.TIM_Prescaler=0;
	TIM_TimeBaseStructure.TIM_ClockDivision= TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse=0;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState=TIM_OCNIdleState_Reset;
	TIM_OC2Init(TIM1,&TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	TIM_OC3Init(TIM1,&TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	TIM_Cmd(TIM1,ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void Motor_Encoder_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;  

	TIM_TimeBaseStructure.TIM_Period=65535;
	TIM_TimeBaseStructure.TIM_Prescaler=0;
	TIM_TimeBaseStructure.TIM_ClockDivision= TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);

	TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter=10;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
	//TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	//TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_SetCounter(TIM2,0);
	TIM_Cmd(TIM2,ENABLE);	

	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);

	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter=10;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	//TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	//TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_SetCounter(TIM3,0);
	TIM_Cmd(TIM3,ENABLE);
}
/***********************************************************************************/
void Motor_Set(uint8_t side, int16_t speed)
{	
	if(speed>3599)        //limit the min and max input value.
	{
		speed=3599;
	}
	else if(speed<-3599)
	{
		speed=-3599;
	}
	//Set Direction & Speed
	if(side==LEFT)
	{
		if(speed>0)
		{
			TIM4->CCR3=speed;
			TIM4->CCR4=0;
		}
		else if(speed<0)
		{
			TIM4->CCR3=0;
			TIM4->CCR4=-speed;
		}
		else
		{
			TIM4->CCR3=0;
			TIM4->CCR4=0;
		}
	}
	else if(side==RIGHT)
	{
		if(speed>0)
		{
			TIM1->CCR2=speed;
			TIM1->CCR3=0;
		}
		else if(speed<0)
		{
			TIM1->CCR2=0;
			TIM1->CCR3=-speed;
		}
		else
		{
			TIM1->CCR2=0;
			TIM1->CCR3=0;
		}	
	}
}

int16_t Read_Encoder(uint8_t TIMX)
{
	int16_t Encoder_TIM;    
	switch(TIMX)
	{
		case 2:
			Encoder_TIM=leftCount;
			break;
		case 3:
			Encoder_TIM=rightCount;
			break;	
		default:  Encoder_TIM=0;
	}
	return Encoder_TIM;
}
