/*
 * buzzer.c
 *
 *  Created on: 2020年5月29日
 *      Author: simonlu
 *     Comment: Music code base from https://blog.csdn.net/qq_36355662/article/details/80606753
 */
 
#include "buzzer.h"

/************************低7  1   2   3   4   5   6   7  高1 高2 高3 高4 高5 不发音***********/
uint16_t basic_tone[] = {247,262,294,330,349,392,440,500,523,587,659,698,784,1000};//音频数据表
/************************ 0   1   2   3   4   5   6   7   8   9   10  11 12    13   **********/


//Nokia Tune
uint8_t nokia_tune[]={9,8,3,5,8};	
uint8_t nokia_tune_time[]={1,1,1,1,2};	

//Beep
uint8_t beep_tune[]={12,13,12};
uint8_t beep_tune_time[]={1,1,1};

void Buzzer_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=BUZZER_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(BUZZER_PORT,&GPIO_InitStructure);
	
	BEEP_LOW();  //S8050 is active high
}

void Sound(uint16_t frq)
{
	uint32_t time;
	if(frq != 1000)
	{
		time = 500000/((uint32_t)frq);
		BEEP_HIGH();
		Delay_us(time);
		BEEP_LOW();
		Delay_us(time);
	}else
		Delay_us(1000);
}

void Play_Music(uint8_t *music,uint8_t *time,uint8_t numb)
{
	uint32_t delay_t;
	uint16_t i,e;
	delay_t = 10;
	for(i=0;i<numb;i++){
		for(e=0;e<((uint16_t)time[i])*basic_tone[music[i]]/delay_t;e++){
			Sound((uint32_t)basic_tone[music[i]]);
		}	
	}
}

void Play_NokiaTune(void)
{
	Play_Music(nokia_tune,nokia_tune_time,5);
}

void Play_Beep(void)
{
	Play_Music(beep_tune,beep_tune_time,3);
}
