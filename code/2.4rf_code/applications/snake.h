/*
 * snake.h
 *
 *  Created on: 2020/7/26
 *      Author: https://www.bilibili.com/video/BV19E411c7y6?from=search&seid=3356520072184532248
 */

#ifndef __SNAKE_H
#define __SNAKE_H

#include "stm32f10x.h"

void Snake_Init(void);

void Snake_Play(void);

static uint8_t Check_ButtonsState(void);
static void debounceButtons(void);	

#endif /* SNAKE_H_ */
