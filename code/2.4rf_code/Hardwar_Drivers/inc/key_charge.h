/*
 * key_charge.h
 *
 *  Created on: 2020Äê7ÔÂ15ÈÕ
 *      Author: simonlu
 */

#ifndef __KEY_CHARGE_H
#define __KEY_CHARGE_H 

#include "stm32f10x.h"

/*
Button Left:   PB11
Button Right:  PA4
	
Charge Detect: PA15
PGood Detect:  PA3	
*/
#define BUTTON_LEFT_PORT       GPIOB
#define BUTTON_RIGHT_PORT      GPIOA
#define CHARGE_DETECT_PORT  	 GPIOA
#define PGOOD_DETECT_PORT   	 GPIOA

#define BUTTON_LEFT_PIN        GPIO_Pin_11
#define BUTTON_RIGHT_PIN       GPIO_Pin_4
#define CHARGE_DETECT_PIN   	 GPIO_Pin_15
#define PGOOD_DETECT_PIN    	 GPIO_Pin_3

void Key_Charge_Init(void);
void Key_Charge_GPIO_Init(void);
void Key_Charge_INT_Init(void);

#endif /* KEY_CHARGE_H_ */
