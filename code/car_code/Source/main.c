/**
  ******************************************************************************
  * @file    main.c
  * @author  Simon Lu
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "hardware.h"
#include "pid.h"

float pitch,roll,yaw;
short gyrox,gyroy,gyroz;
short accex,accey,accez;

int16_t leftEncoder;
int16_t rightEncoder;

int16_t leftSpeed;
int16_t rightSpeed;

extern volatile int16_t carMove;
extern volatile int16_t carTurn;

int16_t maxMove=75;
int16_t maxTurn=2000;

uint16_t rx_data[2]={2040,2040};
//Count every loop until reach a setting value and
//then reset the rx_data if always no data rx.
int16_t timeNoRx=0;  

int main(void)
{	
/****************************************************************
 *	
 *  Prepare
 *
 ***************************************************************/
	Hardware_Init();
	//Rotate make the angle bigger than +/-30 degree 
	//to let the car be quiet when charging.
	mpu_dmp_get_data(&pitch,&roll,&yaw);
	if(roll>30||roll<-30)
	{
		//Play_Beep();
		while(1)
		{
			LED_OFF();
			Delay_ms(500);
			LED_ON();
			Delay_ms(500);
		}
	}
	
/****************************************************************
 *	
 *  Main Loop
 *
 ***************************************************************/
	SI24R1_RxMode();
	Delay_ms(100);
	while(1)
	{
		//Receive Datas from Remote Controller
		if(!SI24R1_RxPacket((uint8_t *)rx_data))
		{
			timeNoRx=0;
		}
		else
		{
			timeNoRx++;
		}
		
		//Determine reset the rx_data or not when reach 500ms.
		if(timeNoRx>=300)
		{
			timeNoRx=0;
			rx_data[0]=2040;
			rx_data[1]=2040;
		}
		
		//Caculate the carMove value
		if(rx_data[0]<2020)
		{
			carMove =(2020-rx_data[0])/(2020/maxMove);
		}
		else if(rx_data[0]>2060)
		{
			carMove=-(rx_data[0]-2060)/(2036/maxMove);
		}
		else{
			carMove=0;
		}
		//Caculate the carTurn value
		if(rx_data[1]<2020)
		{
			carTurn=-(2020-rx_data[1])/(2020/maxTurn);
		}
		else if(rx_data[1]>2060)
		{
			carTurn=(rx_data[1]-2060)/(2036/maxTurn);
		}
		else{
			carTurn=0;
		}
		
		//Car control
		mpu_dmp_get_data(&pitch,&roll,&yaw);
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);
		MPU_Get_Accelerometer(&accex,&accey,&accez);

		leftEncoder=Read_Encoder(LEFT_ENCODER);
		rightEncoder=Read_Encoder(RIGHT_ENCODER);
		
		//Turn off the motors when fall down.
		if(roll>80||roll<-80)
		{
			Motor_Set(LEFT,0);
			Motor_Set(RIGHT,0);
		}
		else
		{
			leftSpeed=balance_UP(roll,gyrox)+velocity(leftEncoder,rightEncoder,roll)-turn(gyroz);
			rightSpeed=balance_UP(roll,gyrox)+velocity(leftEncoder,rightEncoder,roll)+turn(gyroz);
			
			Motor_Set(LEFT,leftSpeed);
			Motor_Set(RIGHT,rightSpeed);
		}
		
		Delay_us(750);
	}
	
}


