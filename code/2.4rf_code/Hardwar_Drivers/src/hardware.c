/*
 * hardware.c
 *
 *  Created on: 2020/7/26
 *      Author: simonlu
 */

#include "hardware.h"

uint8_t temp[5];   //use to translate number to ascii
float BattVolt;
uint16_t JoystickX;
uint16_t JoystickY;

void Hardware_Init(void)
{	
	//OLED Init
	OLED_Init();
	//ÆÁÄ»Ðý×ª180¶È
	OLED_DisplayTurn(1);
	OLED_FillScreen(0);
	OLED_Refresh_Gram();
	printf("OLED DISPLAY ON.\n");
	
	//LED Init
	LED_Init();
	printf("BLUE LED ON.\n");
	
	//Buzzer Init
	Buzzer_Init();
	printf("BUZZER TEST.\n");
	Play_NokiaTune();

	//MPU6050 Init
	while(mpu_dmp_init());
	printf("MPU6050 DMP INIT.\n");
	
	//SI24R1 Init
	//Delay_ms(1000);
	SI24R1_Init();
	//Delay_ms(1000);
	while(SI24R1_Check())
	{
		Delay_ms(1);
	}
	printf("SI24R1 CHECK OK.\n");
	
	//ADC Init
	myADC_Init();
	Delay_ms(5);
	BattVolt=Get_BattVolt();
	printf("BATT VOLT: %.2f\n",BattVolt);
	Delay_ms(500);
	
	JoystickX=Get_JoystickX();
	JoystickY=Get_JoystickY();
	printf("JOYSTICK X: %d\n",JoystickX);
	printf("JOYSTICK Y: %d\n",JoystickY);

	//Key & Charge Detect IO Init
	Key_Charge_Init();
	printf("KEY&CHARGE IO INIT.\n");
	Delay_ms(500);
	
	/*
	OLED_ShowString(0,33,(uint8_t *)("W25Q128 Init."),8,1);
	OLED_Refresh();
	W25Q128_Init();
	while(FLASH_ID_Check())
	{
		LED_OFF();
		Delay_ms(1);
	}
	LED_ON();
	*/
}
