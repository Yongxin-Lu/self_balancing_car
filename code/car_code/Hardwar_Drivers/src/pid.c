/*
 * pid.c
 *
 *  Created on: 2020年6月14日
 *      Author: simonlu
 */

#include "pid.h"

float balance_UP_KP=260;
float balance_UP_KD=1.2;

float velocity_KP=-50;
float velocity_KI=-0.25;

float turn_Kp=-0.7;

volatile int16_t carTurn=0;
volatile int16_t carMove=0;

volatile float Mechanical_balance=0;
volatile float Encoder_Integral=0;

/*****************************************************************
 *                       直立环
 ****************************************************************/
int16_t balance_UP(float Angle,short Gyro)
{  
   float error;
   int32_t balance;
   error=Angle-Mechanical_balance;
   balance=balance_UP_KP*error+balance_UP_KD*Gyro;
   return balance;
}

/*****************************************************************
 *                       速度环
 ****************************************************************/
int16_t velocity(int Encoder_Left,int Encoder_Right,float roll)
{  
    static float Velocity,Encoder_Least,Encoder;

    Encoder_Least =(Encoder_Left+Encoder_Right)-0;      

    Encoder *= 0.7;      
    Encoder += Encoder_Least*0.3;   
    Encoder_Integral +=Encoder;
		Encoder_Integral=Encoder_Integral+carMove;
		
    if(Encoder_Integral>10000)
		{
			Encoder_Integral=10000;   
		}
    if(Encoder_Integral<-10000)
		{
			Encoder_Integral=-10000; 
		}	
		
    Velocity=Encoder*velocity_KP+Encoder_Integral*velocity_KI;  
		
    if(roll<-40||roll>40)
		{
			Encoder_Integral=0;  
		}
    return Velocity;
}

/*****************************************************************
 *                       转向环
 ****************************************************************/
int16_t turn(short Gyro)
{
	float Bias;
	int16_t Turn;
	Bias=Gyro-0;
	Turn=-Bias*turn_Kp;
	Turn+=carTurn;
	return Turn;
}
