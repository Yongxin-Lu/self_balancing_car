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

uint16_t tx_data[2];

int main(void)
{	
	Hardware_Init();
	
	OLED_ClearDisplay();
	OLED_PrintCharEx(0,0,"CONTROL CAR MODE...");
	OLED_Refresh_Gram();
	
	SI24R1_TxMode();
	Delay_ms(100);
	
	while(1)
	{
		if(GPIO_ReadInputDataBit(CHARGE_DETECT_PORT,CHARGE_DETECT_PIN)==Bit_RESET)
		{
			OLED_ClearDisplay();
			printf("BATT VOLT: %.2f\n",Get_BattVolt());
			OLED_Refresh_Gram();
			Delay_ms(2000);
			continue;
		}
		
		tx_data[0]=Get_JoystickY();
		tx_data[1]=Get_JoystickX();
			
		SI24R1_TxPacket((uint8_t *)tx_data);
		Delay_ms(100);
	}
}


