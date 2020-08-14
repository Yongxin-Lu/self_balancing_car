/*
 * snake.c
 *
 *  Created on: 2020/7/26
 *      Author: https://www.bilibili.com/video/BV19E411c7y6/
 */
 
#include "snake.h"
#include "oled.h"
#include "string.h"
#include "adc.h"
#include "buzzer.h"
#include "systick.h"
#include "snake_cover_image.h"

extern float JoystickX;
extern float JoystickY;

void Snake_Init(void)
{
		
}

void Snake_Play(void)
{
	Snake_Init();

	while(1) 
	{				
	}
}

static uint8_t Check_ButtonsState(void)
{
	int16_t x,y;
	x=Get_JoystickX();
	y=Get_JoystickY();
	if(x<1500 || x>2500 || y<1500 || y>2500)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

static void debounceButtons(void)
{ 
	// prevent "noisy" buttons from appearing as multiple presses
  while(!Check_ButtonsState()); // Wait for all keys released
  Delay_ms(100);
}
