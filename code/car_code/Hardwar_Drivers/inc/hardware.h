/*
 * hardware.h
 *
 *  Created on: 2020/8/12
 *      Author: simonlu
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

#include "stm32f10x.h"
#include "led.h"
#include "usart.h"
#include "systick.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "si24r1.h"
#include "volt_adc.h"
#include "drv8833c.h"
#include "buzzer.h"

void Hardware_Init(void);

#endif /* HARDWARE_H_ */
