/*
 * SI24R1.c
 *
 *  Created on: 2020年5月16日
 *      Author: simonlu
 *   Attention: the IRQ is disabled when RX FIFO is full.
 */

#include "si24r1.h"

uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x12,0x34,0x56,0x78,0x90};
uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x12,0x34,0x56,0x78,0x90};

void SI24R1_Init(void)
{
	SI24R1_GPIO_Init();
	SI24R1_SPI_Init();

	SPI2_CSN_HIGH();
	SPI2_CE_LOW();
}

void SI24R1_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//NVIC_InitTypeDef NVIC_InitStructure;
	//EXTI_InitTypeDef EXTI_InitStructure; 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);

	//Init SPI2 CSN Pin, active low, normal high
	//SPI2->CR2 = SPI_CR2_SSOE;                                           //<<------检查这一句是否和之前问题有关
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
	
	//Init SPI2 SCK, MOSI, MISO Pin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	//Init 24R1 CE Pin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_9);

	//Init 24R1 IRQ Pin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	/*
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);
                                                                                           
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 

	NVIC_InitStructure.NVIC_IRQChannel=EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	*/
}

void SI24R1_SPI_Init(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	SPI_Cmd(SPI2,DISABLE);   //<<----------------------------------------------可能很重要
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;    //2.25M
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2,&SPI_InitStructure);

	SPI_Cmd(SPI2,ENABLE);
}

uint8_t SPI2_WriteReadByte(uint8_t data)
{
	 while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	 SPI_I2S_SendData(SPI2,data);

	 while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE));
	 return SPI_I2S_ReceiveData(SPI2);
}

uint8_t SI24R1_Write_Reg(uint8_t reg,uint8_t value)
{
	uint8_t status;

	SPI2_CSN_LOW();
	status=SPI2_WriteReadByte(reg);
	SPI2_WriteReadByte(value);
	SPI2_CSN_HIGH();

	return status;
}

uint8_t SI24R1_Read_Reg(uint8_t reg)
{
	uint8_t reg_val;

	SPI2_CSN_LOW();
	SPI2_WriteReadByte(reg);
	reg_val = SPI2_WriteReadByte(0xff);
	SPI2_CSN_HIGH();

	return reg_val;
}

uint8_t SI24R1_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status;
	int i;

	SPI2_CSN_LOW();
	status = SPI2_WriteReadByte(reg);
	for( i=0;i<len;i++)
	{
		SPI2_WriteReadByte(*pBuf);
		pBuf++;
	}
	SPI2_CSN_HIGH();

	return status;
}

uint8_t SI24R1_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status;
	int i;

	SPI2_CSN_LOW();
	status = SPI2_WriteReadByte(reg);
	for(i = 0;i < len ;i++)
	{
		*pBuf = SPI2_WriteReadByte(0xff);
		pBuf++;
	}
	SPI2_CSN_HIGH();

	return status;
}

void SI24R1_TxMode(void)
{
	SPI2_CE_LOW();
	SI24R1_Write_Buf(W_REGISTER+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);
	SI24R1_Write_Buf(W_REGISTER+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);
	SI24R1_Write_Reg(W_REGISTER+EN_AA,0x01);
	SI24R1_Write_Reg(W_REGISTER+EN_RXADDR,0x01);
	SI24R1_Write_Reg(W_REGISTER+SETUP_RETR,0x05);
	SI24R1_Write_Reg(W_REGISTER+RF_CH,40);
	SI24R1_Write_Reg(W_REGISTER+RF_SETUP,0x0f);
	SI24R1_Write_Reg(W_REGISTER+CONFIG,0x0e);
	SI24R1_Write_Reg(FLUSH_TX,0xff);
	SPI2_CE_HIGH();
}

void SI24R1_RxMode(void)
{
	SPI2_CE_LOW();
	SI24R1_Write_Buf(W_REGISTER+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);
	SI24R1_Write_Reg(W_REGISTER+EN_AA,0x01);
	SI24R1_Write_Reg(W_REGISTER+EN_RXADDR,0x01);
	SI24R1_Write_Reg(W_REGISTER+RF_CH,40);
	SI24R1_Write_Reg(W_REGISTER+RX_PW_P0,RX_PAYLO_WIDTH);
	SI24R1_Write_Reg(W_REGISTER+RF_SETUP,0x0f);
	SI24R1_Write_Reg(W_REGISTER+CONFIG,0x0f);
	SPI2_CE_HIGH();
}

uint8_t SI24R1_TxPacket(uint8_t *txbuf)
{
	uint8_t status;
	SPI2_CE_LOW();
	SI24R1_Write_Buf(W_RX_PAYLOAD,txbuf,TX_PAYLO_WIDTH);
	SPI2_CE_HIGH();
	while(SI24R1_IRQ()!=0);
	status=SI24R1_Read_Reg(STATUS);
	SI24R1_Write_Reg(W_REGISTER+STATUS,status);
	if(status&MAX_TX)
	{
		SI24R1_Write_Reg(FLUSH_TX,0xff);
		return MAX_TX;
	}
	if(status&TX_OK)
	{
		return TX_OK;
	}
	return 0xff;
}

uint8_t SI24R1_RxPacket(uint8_t *rxbuf)
{
	uint8_t status;
	SPI2_CE_LOW();
	status=SI24R1_Read_Reg(STATUS);
	SI24R1_Write_Reg(W_REGISTER+STATUS,status);
	if(status&RX_OK)
	{
		SI24R1_Read_Buf(R_RX_PAYLOAD,rxbuf,RX_PAYLO_WIDTH);
		SI24R1_Write_Reg(FLUSH_RX,0xff);
		return 0;
	}
	SPI2_CE_HIGH();
	return 1;
}

uint8_t SI24R1_Check(void)
{
	uint8_t buf[5]={0xA5,0xA5,0xA5,0xA5,0xA5};
	uint8_t i;
	//SPI2_CE_LOW();
	//Delay_ms(5);
	SI24R1_Write_Buf(W_REGISTER+TX_ADDR,buf,5);
	//Delay_ms(5);
	SI24R1_Read_Buf(TX_ADDR,buf,5);
	//Delay_ms(5);
	for(i=0;i<5;i++)
	{
		if(buf[i]!=0xA5)
		{
			return 1;
		}
	}	
	return 0;
}
