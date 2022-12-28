/*
 * hardwareinit.h
 *
 *  Created on: Dec 25, 2022
 *      Author: user
 */

#ifndef INC_HARDWAREINIT_H_
#define INC_HARDWAREINIT_H_

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_I2C1_Init(void);
void MX_I2C2_Init(void);
void MX_RTC_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART3_UART_Init(void);
void MX_TIM10_Init(void);
void MX_TIM11_Init(void);
void MX_DMA_Init(void);
void MX_SPI1_Init(void);
void MX_I2C3_Init(void);
void MX_ADC1_Init(void);
void Error_Handler(void);
void initmcu(void);

#endif /* INC_HARDWAREINIT_H_ */
