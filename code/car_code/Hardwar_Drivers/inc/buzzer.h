/*
 * buzzer.h
 *
 *  Created on: 2020Äê5ÔÂ29ÈÕ
 *      Author: simonlu
 */
 
#include "stm32f10x.h"
#include "systick.h"

#ifndef BUZZER_H_
#define BUZZER_H_

#define BUZZER_PORT   GPIOA
#define BUZZER_PIN    GPIO_Pin_12

#define BEEP_HIGH() 	GPIO_SetBits(BUZZER_PORT,BUZZER_PIN)
#define BEEP_LOW()	  GPIO_ResetBits(BUZZER_PORT,BUZZER_PIN)

void Buzzer_Init(void);

void Sound(uint16_t frq);
void Play_Music(uint8_t *music,uint8_t *time,uint8_t numb);
void Play_NokiaTune(void);
void Play_Beep(void);

#endif /* BUZZER_H_ */
