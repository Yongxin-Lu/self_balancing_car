#include "mpuiic.h"
#include "systick.h"
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
 
  //MPU IIC 延时函数
void MPU_IIC_Delay(void)
{
	Delay_us(2);
}

//初始化IIC
void MPU_IIC_Init(void)
{					     	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);          //开启GPIOB时钟
	 
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;    	//高速
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11);//PB11,PB12 输出高	
}

//产生IIC起始信号
void MPU_IIC_Start(void)
{
	MPU_SDA_OUT();     //sda线输出
	SET_SDA;	  	  
	SET_SCL;
	MPU_IIC_Delay();
 	CLR_SDA;//START:when CLK is high,DATA change form high to low 
	MPU_IIC_Delay();
	CLR_SCL;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void MPU_IIC_Stop(void)
{
	MPU_SDA_OUT();//sda线输出
	CLR_SCL;
	CLR_SDA;//STOP:when CLK is high DATA change form low to high
 	MPU_IIC_Delay();
	SET_SCL;  
	SET_SDA;//发送I2C总线结束信号
	MPU_IIC_Delay();							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t MPU_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;  
	MPU_SDA_IN();      //SDA设置为输入  
	SET_SDA;MPU_IIC_Delay();	   
	SET_SCL;MPU_IIC_Delay();	 
	while(MPU_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			MPU_IIC_Stop();
			return 1;
		}
	}
	CLR_SCL;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void MPU_IIC_Ack(void)
{
	CLR_SCL;
	MPU_SDA_OUT();
	CLR_SDA;
	MPU_IIC_Delay();
	SET_SCL;
	MPU_IIC_Delay();
	CLR_SCL;
}
//不产生ACK应答		    
void MPU_IIC_NAck(void)
{
	CLR_SCL;
	MPU_SDA_OUT();
	SET_SDA;
	MPU_IIC_Delay();
	SET_SCL;
	MPU_IIC_Delay();
	CLR_SCL;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void MPU_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;  
		MPU_SDA_OUT(); 		
    CLR_SCL;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {   
			if (txd & 0x80) SET_SDA;else CLR_SDA;
      //MPU_IIC_SDA=(txd&0x80)>>7;
      txd<<=1; 	  
			SET_SCL;
			MPU_IIC_Delay(); 
			CLR_SCL;	
			MPU_IIC_Delay();
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t MPU_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	MPU_SDA_IN();//SDA设置为输入
  for(i=0;i<8;i++ )
	{
        CLR_SCL; 
        MPU_IIC_Delay();
		SET_SCL;
        receive<<=1;
        if(MPU_READ_SDA)receive++;   
		MPU_IIC_Delay(); 
    }					 
    if (!ack)
        MPU_IIC_NAck();//发送nACK
    else
        MPU_IIC_Ack(); //发送ACK   
    return receive;
}


















