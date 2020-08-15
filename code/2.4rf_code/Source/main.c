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

#define RF_SLEEP_TIME 4   //4x50=200ms,if joystick don't move over this time, rf chip will stop tx.
#define RX_TIME       40  //40x50=2000ms, send get volt and temp request to car then transfer to rx mode immediately;

void getJoyStickData(void);
void sendData2Car(void);
void getCarTempVolt(void);
void displayGUI(void);

uint16_t tx_data[3]={2040,2040,0};   //joystick y,x,request flag.
float rx_data[2]={0,0};              //mpu temp and battery voltage.
uint8_t time2sleep=0;
uint8_t time2rx=0;

/********************************Main******************************************/
int main(void)
{	
	Hardware_Init();
	
	Play_Beep();
	
	SI24R1_TxMode();
	Delay_ms(1);
	
	while(1)
	{
		getJoyStickData();

		sendData2Car();
		
		getCarTempVolt();
		
	  displayGUI();
		
		Delay_ms(50);
	}
}

/*******************************Get Joystick Data******************************/
void getJoyStickData(void)
{
	tx_data[0]=Get_JoystickY();
	tx_data[1]=Get_JoystickX();
}

/*******************************Send Joystick Data*****************************/
void sendData2Car(void)
{
	if((tx_data[0]<2060&&tx_data[0]>2020)&&(tx_data[1]<2060&&tx_data[1]>2020))
	{
		time2sleep++;
	}
	else{
		time2sleep=0;
	}
	
	if(time2sleep>=RF_SLEEP_TIME)
	{
		time2sleep--;
	}
	else
	{		
		SI24R1_TxPacket((uint8_t *)tx_data);
	}
}

/*********************Get Car's MPU6050 Temp & Battery Voltage*****************/
void getCarTempVolt(void)
{
	if(time2rx>=RX_TIME)
	{
		time2rx=0;
		tx_data[2]=1;
		if(SI24R1_TxPacket((uint8_t *)tx_data)==TX_OK)
		{
			SI24R1_RxMode();
			Delay_ms(1);
			for(int i=0;i<10;i++)
			{
				if(!SI24R1_RxPacket((uint8_t *)rx_data))
				{
					break;
				}
				Delay_ms(1);
			}
			SI24R1_TxMode();
		}
		tx_data[2]=0;
	}
	else
	{
		time2rx++;
	}
}

/********************************Display Main GUI******************************/
void displayGUI(void)
{
	OLED_ClearDisplay();
	
	if(GPIO_ReadInputDataBit(CHARGE_DETECT_PORT,CHARGE_DETECT_PIN)==Bit_RESET)
	{
		;
	}
	printf("%.2f %.2f\n",rx_data[0],rx_data[1]);
	
	OLED_Refresh_Gram();
}
