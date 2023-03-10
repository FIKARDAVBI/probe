/*
 * task.c
 *
 *  Created on: Dec 26, 2022
 *      Author: user
 *
 */
#include "task.h"
#include "kom.h"
#include "hardwareinit.h"
#include "stm32f4xx_hal.h"
#include "inttypes.h"
#include <stdio.h>
#include <stdlib.h>
#include "BME280_STM32.h"
#include "math.h"
#include "MPU6050.h"

#define TEAM_ID 1010

extern RTC_HandleTypeDef hrtc;
extern UART_HandleTypeDef huart3;
extern I2C_HandleTypeDef hi2c2;

uint8_t flagtel=0,flagsim = 0,flagstate = 0,valid = 0;
uint16_t mseconds,counting = 1;
datatelemetri_t datatelemetri;
uint8_t check,csh;
MPU6050_t MPU6050;

float Temperature, Pressure,Humidity,Spressure = 101325,refalt = 0,tempalt = 0;

char commandbuff[15];

void maintask()
{
	ambildata();
	if(flagtel == 1)
	{
		counting++;
		kirimdata();
	}
}

void init()
{
	sprintf(datatelemetri.state,"LAUNCH_WAIT");
	datatelemetri.fmode = 'F';
	datatelemetri.hsdeploy = 'N';
	datatelemetri.pcdeploy = 'N';
	datatelemetri.mastraised = 'N';
	datatelemetri.voltage = 4.123;
	datatelemetri.gpsalt = 20.45;
	datatelemetri.gpslati = 53.8160182;
	datatelemetri.gpslongi = -3.0566566;
	datatelemetri.gpssat = 6;
	sprintf(datatelemetri.echocmd,"CXON");
}

uint8_t buatcs(char dat_[])
{
    uint8_t hasil = 0, temp = 0;
    uint16_t buffhasil = 0;
    for(int i=0;i<150;i++)
    {
        if(dat_[i]== '\0')
        {
        	break;
        }
        else
        {
            buffhasil += dat_[i];
        }
    }
    hasil = buffhasil;
    temp = buffhasil >>8;
    hasil += temp;
    return hasil;
}

float pressuretoalt(float press)
{
	float hasil;
	hasil = 44330 * (1-pow((press/1013.25),(1/5.255)));
	return hasil;
}

void MPUread()
{
	MPU6050_Read_All(&hi2c2, &MPU6050);
}
void ambildata()
{
	get_time();
	datatelemetri.packetcount = counting;
	datatelemetri.temp = Temperature;
	tempalt = datatelemetri.alt;
	if(flagsim == 0 || flagsim == 1)
	{
		datatelemetri.alt = pressuretoalt(Pressure/100);
		datatelemetri.alt -= refalt;
	}
	else if(flagsim == 2)
	{
		datatelemetri.alt = pressuretoalt(Spressure/100);
	}
	if(datatelemetri.alt < 0)
	{
		datatelemetri.alt = 0;
	}
	state();
	datatelemetri.tilt_x = MPU6050.KalmanAngleX;
	datatelemetri.tilt_y = MPU6050.KalmanAngleY;
	clearstring(datatelemetri.telemetri1,36);
	clearstring(datatelemetri.telemetri2,30);
	clearstring(datatelemetri.telemetri3,50);
	clearstring(datatelemetri.telemetri4,50);
	clearstring(datatelemetri.telemetritotal,150);
	clearstring(datatelemetri.telemetribuff,150);
	sprintf(datatelemetri.telemetri1,"%d,%c%c:%c%c:%c%c.%c%c,%d,%c,%s",TEAM_ID,datatelemetri.jam[0],datatelemetri.jam[1],datatelemetri.menit[0],datatelemetri.menit[1],datatelemetri.detik[0],datatelemetri.detik[1],datatelemetri.sentidetik[0],datatelemetri.sentidetik[1],datatelemetri.packetcount,datatelemetri.fmode,datatelemetri.state);
	sprintf(datatelemetri.telemetri2,",%.1f,%c,%c,%c,%.1f",datatelemetri.alt,datatelemetri.hsdeploy,datatelemetri.pcdeploy,datatelemetri.mastraised,datatelemetri.temp);
	sprintf(datatelemetri.telemetri3,",%.1f,%c%c:%c%c:%c%c,%.1f,%.4f,%.4f",datatelemetri.voltage,datatelemetri.gpsjam[0],datatelemetri.gpsjam[1],datatelemetri.gpsmenit[0],datatelemetri.gpsmenit[1],datatelemetri.gpsdetik[0],datatelemetri.gpsdetik[1],datatelemetri.gpsalt,datatelemetri.gpslati,datatelemetri.gpslongi);
	sprintf(datatelemetri.telemetri4,",%d,%.2f,%.2f,%s,,",datatelemetri.gpssat,datatelemetri.tilt_x,datatelemetri.tilt_y,datatelemetri.echocmd);
	sprintf(datatelemetri.telemetribuff,"%s%s%s%s",datatelemetri.telemetri1,datatelemetri.telemetri2,datatelemetri.telemetri3,datatelemetri.telemetri4);
	csh = ~buatcs(datatelemetri.telemetribuff);
	sprintf(datatelemetri.telemetritotal,"%s%d\n",datatelemetri.telemetribuff,csh);
}

void kirimdata()
{
	HAL_UART_Transmit_DMA(&huart3,(uint8_t*)datatelemetri.telemetritotal, sizeof(datatelemetri.telemetritotal));
}

void wakturtc(uint8_t timebuff, char datat[])
{
	if (timebuff < 10)
	{
		sprintf(datat,"0%d",timebuff);
	}
	else
	{
		sprintf(datat,"%d",timebuff);
	}
}

void get_time(void)
{
	RTC_TimeTypeDef gTime;
	RTC_DateTypeDef gDate;

	HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);

	mseconds = (gTime.SubSeconds*1000) / (gTime.SecondFraction + 1);
	mseconds = (999-mseconds)*0.1;

	wakturtc(gTime.Hours,datatelemetri.jam);
	wakturtc(gTime.Minutes,datatelemetri.menit);
	wakturtc(gTime.Seconds,datatelemetri.detik);
	wakturtc(mseconds,datatelemetri.sentidetik);
	wakturtc(gTime.Hours,datatelemetri.gpsjam);
	wakturtc(gTime.Minutes,datatelemetri.gpsmenit);
	wakturtc(gTime.Seconds,datatelemetri.gpsdetik);
}

void Settime(uint8_t jam_, uint8_t menit_, uint8_t detik_)
{
	  HAL_PWR_EnableBkUpAccess();
	  RTC_TimeTypeDef sTime = {0};
	  RTC_DateTypeDef sDate = {0};

	  sTime.Hours = jam_;
	  sTime.Minutes = menit_;
	  sTime.Seconds = detik_;
	  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
	  sDate.Month = RTC_MONTH_JANUARY;
	  sDate.Date = 0x1;
	  sDate.Year = 0x0;

	  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
	  {
	    Error_Handler();
	  }
      HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32BE);
      HAL_PWR_DisableBkUpAccess();
}

void rtcbackup()
{
	  if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1)!= 0x32BE)
	  {
	      Settime(0x0,0x0,0x0);
	  }
}

void clearstring(char data__[],uint8_t ukuran)
{
	int j = 0;
	for(j=0;j<(ukuran-1);j++)
	{
		data__[j] = 0;
	}
}

void CX()
{
	isidata(4,commandbuff);
	if((commandbuff[0] == 'O')&&(commandbuff[1] == 'N'))
		flagtel = 1;
	if((commandbuff[0] == 'O')&&(commandbuff[1] == 'F'))
		flagtel = 0;
	clearstring(datatelemetri.echocmd,15);
	sprintf(datatelemetri.echocmd,"CXON");
}

void ST()
{
	clearstring(commandbuff,15);
	isidata(4,commandbuff);
	uint8_t bufjam,bufmenit,bufdetik;
	char bufjam_[3],bufmenit_[3],bufdetik_[3];
	sprintf(bufjam_,"%c%c",commandbuff[0],commandbuff[1]);
	bufjam = (uint8_t)atoi(bufjam_);
	sprintf(bufmenit_,"%c%c",commandbuff[3],commandbuff[4]);
	bufmenit = (uint8_t)atoi(bufmenit_);
	sprintf(bufdetik_,"%c%c",commandbuff[6],commandbuff[7]);
	bufdetik = (uint8_t)atoi(bufdetik_);
	Settime(bufjam,bufmenit,bufdetik);
	wakturtc(bufjam,bufjam_);
	wakturtc(bufmenit,bufmenit_);
	wakturtc(bufdetik,bufdetik_);
	clearstring(datatelemetri.echocmd,15);
	sprintf(datatelemetri.echocmd,"ST%c%c:%c%c:%c%c",bufjam_[0],bufjam_[1],bufmenit_[0],bufmenit_[1],bufdetik_[0],bufdetik_[1]);
}

void SIM()
{
	isidata(4,commandbuff);
	if(flagsim == 0 && commandbuff[0] == 'E' && commandbuff[1] == 'N')
	{
		clearstring(datatelemetri.echocmd,15);
		flagsim = 1;
		sprintf(datatelemetri.echocmd,"SIMENABLE");
	}
	else if(flagsim == 1 && commandbuff[0] == 'A' && commandbuff[1] == 'C')
	{
		clearstring(datatelemetri.echocmd,15);
		flagsim = 2;
		datatelemetri.fmode = 'S';
		sprintf(datatelemetri.echocmd,"SIMACTIVATE");

	}
	else if(commandbuff[0] == 'D' && commandbuff[1] == 'I')
	{
		clearstring(datatelemetri.echocmd,15);
		flagsim = 0;
		datatelemetri.fmode = 'F';
		sprintf(datatelemetri.echocmd,"SIMDISABLE");
	}
}

void SIMP()
{
	if(flagsim == 2)
	{
		clearstring(datatelemetri.echocmd,15);
		clearstring(commandbuff,15);
		isidata(4,commandbuff);
		Spressure = atof(commandbuff);
		sprintf(datatelemetri.echocmd,"SIMP%.2f",Spressure);
	}
}

void CAL()
{
	if(flagsim == 0)
		refalt = pressuretoalt(Pressure/100);
	flagtel = 0;
	init();
	flagsim = 0;
}

void state()
{
	if(datatelemetri.alt > 100 && flagstate == 0)
	{
		sprintf(datatelemetri.state,"ASCENT");
		flagstate = 1;
	}
	else if((datatelemetri.alt - tempalt) < 0 && flagstate == 1)
	{
		valid++;
		if(valid == 2)
		{
			sprintf(datatelemetri.state,"DESCENT");
			flagstate = 2;
			valid = 0;
		}
	}
	else if(datatelemetri.alt < 500 && flagstate == 2)
	{
		sprintf(datatelemetri.state,"HS_DEPLOYED");
		flagstate = 3;
		//mulai rekam kamera, menjalankan mekanisme rilis payload dan buka heatshield
	}
	else if(datatelemetri.alt < 200 && flagstate == 3)
	{
		sprintf(datatelemetri.state,"PC_DEPLOYED");
		flagstate =4;
		//menjalankan mekanisme membuka parasut
	}
	else if(datatelemetri.alt < 13 && flagstate == 4)
	{
		sprintf(datatelemetri.state,"LANDED");
		flagstate =5;
		//kamera mat,buzzernyala,mulai mekanisme upright
	}
}

