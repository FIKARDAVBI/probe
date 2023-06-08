/*
 * servo.c
 *
 *  Created on: Mar 14, 2023
 *      Author: user
 */
#include "servo.h"
#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim4;

void servogerak(uint16_t sudut)
{
	uint16_t regisval;
	if(sudut == 0){
		regisval = 500;
	}
	else{
		regisval = 11.17*sudut + 490.7;
	}
	TIM4->CCR1 = regisval;
	HAL_TIM_PWM_Start_DMA(&htim4, TIM_CHANNEL_1);
}

