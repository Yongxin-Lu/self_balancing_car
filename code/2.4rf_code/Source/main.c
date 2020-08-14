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

#define RF_SLEEP_TIME 4   //if joystick don't move over this time, rf chip will stop tx.

uint16_t tx_data[2];
uint8_t time2count=0;

int main(void)
{	
	Hardware_Init();
	
	SI24R1_TxMode();
	Delay_ms(100);
	
	while(1)
	{
		//Work in charging mode.
		if(GPIO_ReadInputDataBit(CHARGE_DETECT_PORT,CHARGE_DETECT_PIN)==Bit_RESET)
		{
			OLED_ClearDisplay();
			printf("BATT VOLT: %.2f\n",Get_BattVolt());
			OLED_Refresh_Gram();
			Delay_ms(2000);
			continue;
		}
		
		//Main work.
		tx_data[0]=Get_JoystickY();
		tx_data[1]=Get_JoystickX();
		
		if((tx_data[0]<2060&&tx_data[0]>2020)&&(tx_data[1]<2060&&tx_data[1]>2020))
		{
			time2count++;
		}
		else{
			time2count=0;
		}

		if(time2count>=RF_SLEEP_TIME)
		{
			time2count--;
			OLED_ClearDisplay();
			OLED_PrintCharEx(0,0,"SLEEP MODE...");
			OLED_Refresh_Gram();
		}
		else
		{		
			SI24R1_TxPacket((uint8_t *)tx_data);
			
			OLED_ClearDisplay();
			OLED_PrintCharEx(0,0,"TX ING...");
			OLED_Refresh_Gram();
		}
		
		Delay_ms(50);
	}
}


