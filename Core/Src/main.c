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
#include "servo.h"
#include "tm_stm32f4_bkpsram.h"

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
I2C_HandleTypeDef hi2c2;
I2C_HandleTypeDef hi2c3;
RTC_HandleTypeDef hrtc;
SPI_HandleTypeDef hspi1;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim10;
TIM_HandleTypeDef htim11;
TIM_HandleTypeDef htim13;
DMA_HandleTypeDef hdma_tim4_ch1;
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
char namafile[15];
uint8_t flaginitmotor;
uint8_t flagkuncupmotor;
uint8_t flagmotor;
uint8_t timermotor;
uint8_t flagkameraon;
uint8_t timerkamera1 = 29;
uint8_t flagkameraoff;
uint8_t timerkamera2 = 26;
uint8_t flagupright;
uint8_t flagbukaprobe;
uint8_t timerbukaprobe = 6;
uint8_t flagdoneupright;

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
		if(flaginitmotor)
		{
			if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)){
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, SET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, RESET);
			}
			else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)){
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, RESET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, RESET);
				flaginitmotor = 0;
			}
		}

		if(flagkuncupmotor)
		{
			if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)){
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, SET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, RESET);
			}
			else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)){
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, RESET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, RESET);
				flagkuncupmotor = 0;
				flagbukaprobe = 1;
			}

		}

		if(flagupright){
			if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)){
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, RESET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, SET);
			}
			else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)){
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, RESET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, RESET);
				flagupright = 0;
				flagdoneupright = 1;
			}
		}
	}

	if (htim == &htim10)
	{
		parsinggpsdata();
		maintask();
	    /* USER CODE BEGIN 3 */
	}

	if (htim == &htim11)
	{
		BME280_Measure();
		MPUread();
		if(flagmotor){
			timermotor--;
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, RESET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, SET);
			if(timermotor < 1){
				flagmotor = 0;
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, RESET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, RESET);
			}
		}

		if(flagdoneupright){
			servogerak(50);
			flagdoneupright = 0;
		}

		if(flagkameraon){
			timerkamera1--;
			if(timerkamera1 < 29 && timerkamera1 > 27){
				//28
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, SET);
			}
			else if(timerkamera1 > 11 && timerkamera1 < 13){
				//12
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, RESET);
			}
			else if(timerkamera1 > 3 && timerkamera1 < 5){
				//4
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, SET);
			}
			if(timerkamera1 < 1){
				flagkameraon = 0;
				timerkamera1 = 29;
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, RESET);

			}
		}


		if(flagkameraoff)
		{
			timerkamera2--;
			if(timerkamera2 < 26 && timerkamera2 > 24){
				//25
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, SET);
			}
			else if(timerkamera2 < 22 && timerkamera2 > 20){
				//21
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, RESET);
			}
			else if(timerkamera2 < 19 && timerkamera2 > 17){
				//18
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, SET);
			}
			else if(timerkamera2 < 1){
				flagkameraoff = 0;
				timerkamera2 = 26;
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, RESET);
			}
		}

		if(flagbukaprobe){
			timerbukaprobe--;
			if(timerbukaprobe < 1){
				timermotor = 28;
				flagmotor = 1;
				flagbukaprobe = 0;
				timerbukaprobe = 6;
			}
		}
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
	TM_BKPSRAM_Init();
	init();
	rtcbackup();
	while (MPU6050_Init(&hi2c2) == 1);
	BME280_Config(OSRS_2, OSRS_16, OSRS_1, MODE_NORMAL, T_SB_0p5, IIR_16);
	kominit();
	MX_FATFS_Init();
	fresult = f_mount(&fs,"",1);
	adcinit();
	gpsinit();
	HAL_TIM_Base_Start_IT(&htim11);
	HAL_TIM_Base_Start_IT(&htim13);
	HAL_TIM_Base_Start_IT(&htim10);
	//sprintf(namafile,"%d.txt",TM_BKPSRAM_Read16(0x190));
	while (1)
	{
		if(flagsimpan)
		{
			fresult = f_open(&fil, "1.txt", FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
			fresult = f_lseek(&fil,f_size(&fil));
			fresult = f_write(&fil,buffer,bufsize(buffer),&bw);
			f_close(&fil);
			flagsimpan = 0;
		}
	    /* USER CODE END WHILE */
	}
}
