/*
 * task.h
 *
 *  Created on: Dec 26, 2022
 *      Author: user
 */

#ifndef INC_TASK_H_
#define INC_TASK_H_

#include "stm32f407xx.h"

void rtcbackup();
void Settime(uint8_t jam_, uint8_t menit_, uint8_t detik_);
void CX();
void ST();
void SIM();
void SIMP();
void maintask();
void get_time(void);
void CAL();
void kirimdata();
void wakturtc(uint8_t timebuff, char datat[]);
void ambildata();
void init();
void clearstring(char data__[],uint8_t ukuran);

typedef struct {
	char telemetri1[36];
	char telemetri2[30];
	char telemetri3[50];
	char telemetri4[50];
	char telemetritotal[150];

	char jam[2];
	char menit[2];
	char detik[2];
	char sentidetik[2];

	uint16_t packetcount;

	char fmode;
	char state[15];

	float alt;

	char hsdeploy;
	char pcdeploy;
	char mastraised;

	float temp;
	float voltage;

	char gpsjam[2];
	char gpsmenit[2];
	char gpsdetik[2];

	float gpsalt;
	float gpslati;
	float gpslongi;

	uint8_t gpssat;

	float tilt_x;
	float tilt_y;

	char echocmd[15];
} datatelemetri_t;

#endif /* INC_TASK_H_ */
