/*
 * mpu6050.h
 *
 *  Created on: 2020年5月13日
 *      Author: 踏雪无痕
 *        Edit: simon
 */

#include "stm32f10x.h"
#include "systick.h"

#ifndef MPU6050_H_
#define MPU6050_H_

#define MPU6050_ADDRESS 0xd0

#define MPU_ADDR 0x68
#define MPU_WHO_AM_I 0x75
#define MPU_PWR_MGMT1 0x6B
#define MPU_PWR_MGMT2 0x6C
#define MPU_INT_EN 0x38
#define MPU_USER_CTRL 0x6A
#define MPU_FIFO_EN 0x23
#define MPU_INTBP_CONFIG 0x37
#define MPU_SAMPLE_RATE 0x19
#define MPU_CONFIG 0x1A
#define MPU_GYRO_CONFIG 0x1B
#define MPU_ACCEL_CONFIG 0x1C

#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40

#define TEMP_OUT_H  0x41
#define TEMP_OUT_L  0x42

#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48

#define NOP() Delay_us(3)
#define SET_SCL 	GPIOB->BSRR=GPIO_BSRR_BS10
#define SET_SDA 	GPIOB->BSRR=GPIO_BSRR_BS11
#define CLR_SCL 	GPIOB->BSRR=GPIO_BSRR_BR10
#define CLR_SDA 	GPIOB->BSRR=GPIO_BSRR_BR11
//#define SDA_IN  	GPIOB->CRH&=0xffff0fff;GPIOB->CRH|=0x00008000
//#define SDA_OUT 	GPIOB->CRH&=0xffff0fff;GPIOB->CRH|=0x00003000
#define READ_SDA 	GPIOB->IDR&GPIO_IDR_IDR11

void I2C_Start(void);
void I2C_Stop(void);
void Master_ACK(uint8_t i);
uint8_t Test_ACK(void);
void I2C_SendByte(uint8_t byte);
uint8_t I2C_ReadByte(void);

uint8_t I2C_ReadData(uint8_t REG_Address);
void I2C_WriteData(uint8_t REG_Address,uint8_t Data);

uint8_t MPU6050_Init(void);
int16_t MPU_GetData(uint8_t REG_Address);
void MPU_Set_Gyro_Fsr(uint8_t fsr);
void MPU_Set_Accel_Fsr(uint8_t fsr);
void MPU_Set_LPF(uint16_t lpf);
void MPU_Set_Rate(uint16_t rate);

#endif /* MPU6050_H_ */
