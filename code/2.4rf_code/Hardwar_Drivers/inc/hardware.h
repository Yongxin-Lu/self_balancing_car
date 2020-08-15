/*
 * hardware.h
 *
 *  Created on: 2020/7/26
 *      Author: simonlu
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

#include "stm32f10x.h"
#include "led.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "si24r1.h"
#include "adc.h"
#include "buzzer.h"
#include "oled.h"
#include "key_charge.h"
#include "w25q128.h"
#include "systick.h"

void Hardware_Init(void);

#endif /* HARDWARE_H_ */
