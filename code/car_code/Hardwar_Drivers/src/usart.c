/*
 * usart.c
 *
 *  Created on: 2020年5月15日
 *      Author: simonlu
*/

#include "usart.h"

/********** 禁用半主机模式 **********/
#pragma import(__use_no_semihosting)
 
struct __FILE
{
	int a;
};
 
FILE __stdout;
 
void _sys_exit(int x)
{
}

int fputc(int ch, FILE *f)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	USART_SendData(USART1, (unsigned char)ch);
	return ch;
}	

void USART1_Init(void)
{
	USART1_GPIO_Init();

	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_AFIO,ENABLE);

	USART_InitStructure.USART_BaudRate=USART_BANDRATE;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure);
	USART_Cmd(USART1,ENABLE);
}

void USART1_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//Remap pins PB6 PB7 as USART1
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);

	//Config USART Tx pin PB6 as Pull Push;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	//Config USART1 RX pin PB7 as input floating
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

void USART1_SendByte(uint8_t data)
{
	USART_SendData(USART1,data);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
}

void USART1_SendStr(uint8_t *data)
{
	while(*data)
	{
		USART1_SendByte(*data++);
	}
}

int __io_putchar(int ch)
{
    USART1_SendStr((uint8_t *)&ch);
    return ch;
}
