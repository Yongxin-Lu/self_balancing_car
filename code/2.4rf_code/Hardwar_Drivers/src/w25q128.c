/*
 * w25q128.c
 *
 *  Created on: 2020Äê7ÔÂ15ÈÕ
 *      Author: simonlu
 */

#include "w25q128.h"

void W25Q128_Init(void)
{
	FLASH_GPIO_Init();
	FLASH_SPI_Init();
}

void FLASH_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = FLASH_CHIP_PIN;           //flash chip csn pin
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(FLASH_CHIP_PORT, &GPIO_InitStructure);
 	GPIO_SetBits(FLASH_CHIP_PORT,FLASH_CHIP_PIN);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;           //OLED csn pin
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_9);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void FLASH_SPI_Init(void)
{	
	SPI_InitTypeDef SPI_InitStructure;
	
	GPIO_PinRemapConfig(GPIO_Remap_SPI1,ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	SPI_Cmd(SPI1,DISABLE);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1,&SPI_InitStructure);

	SPI_Cmd(SPI1,ENABLE);
	SPI1_WriteReadByte(0xFF);
}

uint8_t SPI1_WriteReadByte(uint8_t data)
{
	 while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
	 SPI_I2S_SendData(SPI1,data);

	 while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
	 return SPI_I2S_ReceiveData(SPI1);
}

uint8_t FLASH_ID_Check(void)
{
	uint8_t flash_id[2];
	
	FLASH_CSN_LOW();
	
	SPI1_WriteReadByte(0x90);
	SPI1_WriteReadByte(0x0);
	SPI1_WriteReadByte(0x0);
	SPI1_WriteReadByte(0x0);
	
	flash_id[0]=SPI1_WriteReadByte(0xff);
	flash_id[1]=SPI1_WriteReadByte(0xff);
	
	FLASH_CSN_HIGH();
	
	if(flash_id[0]==MANUFACTURERID && flash_id[1]==DEVICEID)
	{
		return 0;
	}else
	{
		return 1;
	}
}

