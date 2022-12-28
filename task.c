/*
 * task.c
 *
 *  Created on: Dec 26, 2022
 *      Author: user
 */
#include "task.h"
#include "kom.h"
#include "hardwareinit.h"
#include "stm32f4xx_hal.h"
#include "inttypes.h"
#include "stdio.h"
#define TEAM_ID 1010

extern RTC_HandleTypeDef hrtc;
extern UART_HandleTypeDef huart3;

uint8_t flagtel=0;
uint16_t mseconds;
datatelemetri_t datatelemetri;
char commandbuff[15];


void maintask()
{
	ambildata();

	if(flagtel == 1)
	{
		kirimdata();
	}
}

void ambildata()
{
	get_time();
	datatelemetri.packetcount++;
	sprintf(datatelemetri.state,"LAUNCH");
	datatelemetri.fmode = 'F';
	datatelemetri.hsdeploy = 'N';
	datatelemetri.pcdeploy = 'N';
	datatelemetri.mastraised = 'N';
	datatelemetri.alt = 10.33;
	datatelemetri.temp = 9.22;
	datatelemetri.voltage = 4.123;
	datatelemetri.gpsalt = 20.45;
	datatelemetri.gpslati = 53.8160182;
	datatelemetri.gpslongi = -3.0566566;
	datatelemetri.gpssat = 6;
	datatelemetri.tilt_x = 47.444;
	datatelemetri.tilt_y = 10.66;
	sprintf(datatelemetri.echocmd,"SIMP101325");
	sprintf(datatelemetri.telemetri1,"%d,%c%c:%c%c:%c%c.%c%c,%d,%c,%s",TEAM_ID,datatelemetri.jam[0],datatelemetri.jam[1],datatelemetri.menit[0],datatelemetri.menit[1],datatelemetri.detik[0],datatelemetri.detik[1],datatelemetri.sentidetik[0],datatelemetri.sentidetik[1],datatelemetri.packetcount,datatelemetri.fmode,datatelemetri.state);
	sprintf(datatelemetri.telemetri2,",%.1f,%c,%c,%c,%.1f",datatelemetri.alt,datatelemetri.hsdeploy,datatelemetri.pcdeploy,datatelemetri.mastraised,datatelemetri.temp);
	sprintf(datatelemetri.telemetri3,",%.1f,%c%c:%c%c:%c%c,%.1f,%.4f,%.4f",datatelemetri.voltage,datatelemetri.gpsjam[0],datatelemetri.gpsjam[1],datatelemetri.gpsmenit[0],datatelemetri.gpsmenit[1],datatelemetri.gpsdetik[0],datatelemetri.gpsdetik[1],datatelemetri.gpsalt,datatelemetri.gpslati,datatelemetri.gpslongi);
	sprintf(datatelemetri.telemetri4,",%d,%.2f,%.2f,%s\n",datatelemetri.gpssat,datatelemetri.tilt_x,datatelemetri.tilt_y,datatelemetri.echocmd);
	sprintf(datatelemetri.telemetritotal,"%s%s%s%s",datatelemetri.telemetri1,datatelemetri.telemetri2,datatelemetri.telemetri3,datatelemetri.telemetri4);
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

void Settime()
{
	  RTC_TimeTypeDef sTime = {0};
	  RTC_DateTypeDef sDate = {0};
	  sTime.Hours = 0x0;
	  sTime.Minutes = 0x0;
	  sTime.Seconds = 0x0;
	  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
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
}

void rtcbackup()
{
	  if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1)!= 0x32BE)
	  {
	      HAL_PWR_EnableBkUpAccess();
	      Settime();
	      HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32BE);
	      HAL_PWR_DisableBkUpAccess();
	  }
}

void CX()
{
	isidata(4,commandbuff);
	if((commandbuff[0] == 'O')&&(commandbuff[1] == 'N'))
		flagtel = 1;
	if((commandbuff[0] == 'O')&&(commandbuff[1] == 'F'))
		flagtel = 0;
}

void ST()
{

}

void SIM()
{

}

void SIMP()
{

}

void CAL()
{

}

