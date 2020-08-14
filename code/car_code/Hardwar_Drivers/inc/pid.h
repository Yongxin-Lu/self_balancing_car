/*
 * pid.h
 *
 *  Created on: 2020Äê5ÔÂ16ÈÕ
 *      Author: simonlu
 */

#include "stm32f10x.h"

#ifndef PID_H_
#define PID_H_

int16_t balance_UP(float Angle,short Gyro);
int16_t velocity(int Encoder_Left,int Encoder_Right,float roll);
int16_t turn(short Gyro);

#endif /* PID_H_ */
