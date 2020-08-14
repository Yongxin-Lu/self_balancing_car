/*
 * hardware.h
 *
 *  Created on: 2020/8/12
 *      Author: simonlu
 *      Edited: 08/14
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

#define SLEEP_TIME       200      //ms, car will stop moving if doesn't rx data over this time.
#define STOP_TIME    900000   //ms, =15mins, motors will shutdown if doesn't rx data over this time.
#define VOLT_STOP_VALUE  3.3      //v
#define FALL_ALARM_TIME  800      //ms, beep() once per this time.

void Hardware_Init(void);

#endif /* HARDWARE_H_ */
