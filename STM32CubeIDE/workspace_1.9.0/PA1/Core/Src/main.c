#include "main.h"
#include "fatfs.h"
#include "hardwareinit.h"
#include <stdio.h>
#include "kom.h"
#include "task.h"
#include "BME280_STM32.h"
#include "MPU6050.h"
#include "gps.h"
#include "fatfs_sd.h"
#include "string.h"

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;
I2C_HandleTypeDef hi2c3;
RTC_HandleTypeDef hrtc;
SPI_HandleTypeDef hspi1;
TIM_HandleTypeDef htim10;
TIM_HandleTypeDef htim11;
TIM_HandleTypeDef htim13;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

FATFS fs;
FIL fil;
FRESULT fresult;

char buffer[1024];

UINT br,bw;

FATFS *pfs;
DWORD fre_clust;
uint32_t total,free_space;
uint8_t flagsimpan = 0;

int bufsize (char *buf)
{
	int i=0;
	while(*buf++ != '\0') i++;
	return i;
}

void bufclear(void)
{
	for (int i = 0; i<1024; i++)
	{
		buffer[i] ='\0';
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	ADC_measure();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim13)
	{
		BME280_Measure();
	}
	if (htim == &htim10)
	{
		parsinggpsdata();
		maintask();
	    /* USER CODE BEGIN 3 */
	}

	if (htim == &htim11)
	{
		MPUread();
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
	{
		checkgpsdata();
	}
	if(huart == &huart2)
	{
		checkdata_();
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}

int main(void)
{
	HAL_Init();
	initmcu();
	init();
	rtcbackup();
	BME280_Config(OSRS_2, OSRS_16, OSRS_1, MODE_NORMAL, T_SB_0p5, IIR_16);
	while (MPU6050_Init(&hi2c2) == 1);
	kominit();
	MX_FATFS_Init();
	fresult = f_mount(&fs,"",0);
	adcinit();
	HAL_TIM_Base_Start_IT(&htim11);
	HAL_Delay(1000);
	HAL_TIM_Base_Start_IT(&htim10);
	HAL_Delay(1000);
	HAL_TIM_Base_Start_IT(&htim13);
	gpsinit();
	while (1)
	{
		if(flagsimpan)
		{
			fresult = f_open(&fil, "file17.txt", FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
			fresult = f_lseek(&fil,f_size(&fil));
			fresult = f_write(&fil,buffer,bufsize(buffer),&bw);
			f_close(&fil);
			flagsimpan = 0;
		}
	    /* USER CODE END WHILE */
	}
}
