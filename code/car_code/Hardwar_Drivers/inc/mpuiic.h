#ifndef __MPUIIC_H
#define __MPUIIC_H
#include "stm32f10x.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK NANO STM32F103开发板
//MPU6050 IIC驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/7/28
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2018-2028
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
	   		   
//IO方向设置
#define MPU_SDA_IN()	 	GPIOB->CRH&=0xffff0fff;GPIOB->CRH|=0x00008000
#define MPU_SDA_OUT() 	GPIOB->CRH&=0xffff0fff;GPIOB->CRH|=0x00003000

//IO操作函数	 
#define SET_SCL 	GPIOB->BSRR=GPIO_BSRR_BS10
#define SET_SDA 	GPIOB->BSRR=GPIO_BSRR_BS11
#define CLR_SCL 	GPIOB->BSRR=GPIO_BSRR_BR10
#define CLR_SDA 	GPIOB->BSRR=GPIO_BSRR_BR11

#define MPU_READ_SDA GPIOB->IDR&GPIO_IDR_IDR11

//IIC所有操作函数
void MPU_IIC_Delay(void);				//MPU IIC延时函数
void MPU_IIC_Init(void);                //初始化IIC的IO口				 
void MPU_IIC_Start(void);				//发送IIC开始信号
void MPU_IIC_Stop(void);	  			//发送IIC停止信号
void MPU_IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t MPU_IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t MPU_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void MPU_IIC_Ack(void);					//IIC发送ACK信号
void MPU_IIC_NAck(void);				//IIC不发送ACK信号

void IMPU_IC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t MPU_IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	  
#endif
















