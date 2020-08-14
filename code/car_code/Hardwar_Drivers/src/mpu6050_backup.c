/*
 * mpu6050.c

 *
 *  Created on: 2020年5月13日
 *      Author: 踏雪无痕
 *        Edit: simon
 */

#include "mpu6050.h"

uint8_t MPU6050_Init(void)
{	
	uint8_t res; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	//I2C SCL, SDA pin Init
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11);
	
	//MPU6050 INT pin Init
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	I2C_WriteData(MPU_PWR_MGMT1,0x80); 	//Reset mpu6050
	Delay_ms(100);
	I2C_WriteData(MPU_PWR_MGMT1,0x00); 	//Wakeup mpu6050
	MPU_Set_Gyro_Fsr(3);                //陀螺仪传感器,±2000dps
  MPU_Set_Accel_Fsr(0);               //加速度传感器,±2g
  MPU_Set_Rate(50);
	I2C_WriteData(MPU_INT_EN,0X00);			//关闭所有中断
	I2C_WriteData(MPU_USER_CTRL,0X00);  //I2C主模式关闭
	I2C_WriteData(MPU_FIFO_EN,0X00);		//关闭FIFO
	I2C_WriteData(MPU_INTBP_CONFIG,0X80);
	res=I2C_ReadData(MPU_WHO_AM_I);
	if(res==0x68)//器件ID正确
	{
		I2C_WriteData(MPU_PWR_MGMT1,0X01);//设置CLKSEL,PLL X轴为参考
		I2C_WriteData(MPU_PWR_MGMT2,0X00);//加速度与陀螺仪都工作
		MPU_Set_Rate(50);//设置采样率为50Hz
	}else return 1;
	return 0;
}

void I2C_Start(void)  
{
	//SDA_OUT;
	SET_SCL;
	NOP();
	SET_SDA;
	Delay_us(5);
	CLR_SDA;
	Delay_us(5);
}

void I2C_Stop(void)
{
	//SDA_OUT;
	CLR_SDA;
	NOP();
	SET_SCL;
	Delay_us(5);
	SET_SDA;
	Delay_us(5);
}

void Master_ACK(uint8_t i)		
{
	//SDA_OUT;
	CLR_SCL; 					// 拉低时钟总线允许SDA数据总线上的数据变化
	NOP(); 						// 让总线稳定
	if(i) CLR_SDA;else SET_SDA;	//如果i = 1 那么拉低数据总线 表示主机应答
	NOP();						//让总线稳定
	SET_SCL;					//拉高时钟总线 让从机从SDA线上读走 主机的应答信号
	NOP();
	CLR_SCL;					//拉低时钟总线， 占用总线继续通信
	NOP();
	SET_SDA;					//释放SDA数据总线。
	NOP();
}

uint8_t Test_ACK(void)
{
	//SDA_IN;
	SET_SCL;
	NOP();
	if (READ_SDA)
	{
		CLR_SCL;
		NOP();
		I2C_Stop();
		return(0);
	}
	else
	{
		CLR_SCL;
		NOP();
		return(1);
	}
}

void I2C_SendByte(uint8_t byte)
{
	uint8_t i;
	//SDA_OUT;
	for(i = 0 ; i < 8 ; i++)
	{
		CLR_SCL;
		NOP();
		if (byte & 0x80) SET_SDA;else CLR_SDA;
		NOP();
		SET_SCL;
		NOP();
		byte <<= 1;	
	}
	CLR_SCL;
	NOP();
	SET_SDA;
	NOP();
}

uint8_t I2C_ReadByte(void)
{
	uint8_t dat,i;
	CLR_SCL;
	NOP();
	//SDA_IN;
	NOP();
	for(i = 0 ; i < 8 ; i++)
	{
		dat = dat << 1;	
		SET_SCL;
		NOP();
		if (READ_SDA) dat |= 0x01; 
		NOP();
		CLR_SCL;
		NOP();
	}
	return(dat);
}

uint8_t I2C_ReadData(uint8_t REG_Address)
{
	uint8_t Data;
	I2C_Start();
	I2C_SendByte(MPU6050_ADDRESS);
	Test_ACK();
	I2C_SendByte(REG_Address);
	Test_ACK();
	I2C_Start();
	I2C_SendByte(MPU6050_ADDRESS+1);
	Test_ACK();
	Data=I2C_ReadByte();
	Master_ACK(0);
	I2C_Stop();
	return Data;
}

void I2C_WriteData(uint8_t REG_Address,uint8_t Data)
{
	I2C_Start();
	I2C_SendByte(MPU6050_ADDRESS);
	Test_ACK();
	I2C_SendByte(REG_Address);
	Test_ACK();
	I2C_SendByte(Data);
	Test_ACK();
	I2C_Stop();
}


int16_t MPU_GetData(uint8_t REG_Address)
{
	int8_t H,L;
	H=I2C_ReadData(REG_Address);
	L=I2C_ReadData(REG_Address+1);
	return (H<<8)+L;
}

//设置MPU6050陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
void MPU_Set_Gyro_Fsr(uint8_t fsr)
{
	I2C_WriteData(MPU_GYRO_CONFIG,fsr<<3); 
}

//设置MPU6050加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
void MPU_Set_Accel_Fsr(uint8_t fsr)
{
	I2C_WriteData(MPU_ACCEL_CONFIG,fsr<<3);
}

//设置MPU6050的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
void MPU_Set_LPF(uint16_t lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6;
	I2C_WriteData(MPU_CONFIG,data);
}

//设置MPU6050的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
void MPU_Set_Rate(uint16_t rate)
{
	uint8_t data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	I2C_WriteData(MPU_SAMPLE_RATE,data);
	MPU_Set_LPF(rate/2); //自动设置LPF为采样率的一半
}
