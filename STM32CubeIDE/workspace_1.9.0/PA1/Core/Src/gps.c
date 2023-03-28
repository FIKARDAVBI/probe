/*
 * gps.c
 *
 *  Created on: Mar 6, 2023
 *      Author: user
 */
#include "gps.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

uint8_t rxgpsdata[2],datagpslkp[100],i_,cmd_,gpslongraw[11],gpslatraw[11],gpsaltraw[10],gpstimeraw[15],gpssatraw[10],gpsqual[1],gpslathead[1],gpslonghead[1];

extern float gpslat,gpslong,gpsalt;
extern uint8_t gpssat;
extern char gpsdetik[3],gpsmenit[3],gpsjam[3];

extern UART_HandleTypeDef huart1;
uint8_t checksum;

void parsedata(uint8_t urutan_ ,uint8_t data[])
{
	clearchararray(data,strlen((char*)data));
	uint8_t j = 0,counter = 0;
	for(int k = 0;k<strlen((char*)datagpslkp);k++)
	{
		if(datagpslkp[k] == ',')
		{
		    counter++;
			if(counter == urutan_-1)
			{
				data[j] = '0';
			}
		}
		else
		{
			if(counter == urutan_-1)
			{
				data[j] = datagpslkp[k];
				j++;
			}
		}
	}
}

void clearchararray(uint8_t* data_, int len)
{
	for(int i = 0;i<len;i++){
		data_[i] = '\0';
	}
}

uint8_t checkNMEAGPGGA(uint8_t* data) {
    uint32_t i;
    uint8_t calculated_checksum = 0;
    unsigned int n;

    uint8_t* checksum_start = (uint8_t*)strrchr((const char*)data, '*');
    if (checksum_start == NULL) {
        return 0;
    }

    for (i = 1; &data[i] < checksum_start; i++) {
        calculated_checksum ^= data[i];
    }

    if (sscanf((const char*)(checksum_start + 1), "%2x", &n) != 1) {
        return 0;
    }

    if (calculated_checksum == n) {
        return 1;
    } else {
        return 0;
    }
}

void checkgpsdata()
{
	HAL_UART_Receive_DMA(&huart1, (uint8_t*)rxgpsdata, 1);
	if(rxgpsdata[0] == '$')
	{
		datagpslkp[i_] = rxgpsdata[0];
		i_++;
		cmd_++;
	}
	else if(cmd_ > 0)
	{
		if(rxgpsdata[0] != '\r')
		{
			datagpslkp[i_] = rxgpsdata[0];
			i_++;
		}
		else
		{
			if(datagpslkp[3] == 'G' && datagpslkp[4] == 'G' &&datagpslkp[5] == 'A')
			{
				if(checkNMEAGPGGA(datagpslkp)){
					parsedata(3,gpslatraw);
					parsedata(5,gpslongraw);
					parsedata(10,gpsaltraw);
					parsedata(4,gpslathead);
					parsedata(6,gpslonghead);
					parsedata(2,gpstimeraw);
					parsedata(8,gpssatraw);
					parsedata(7,gpsqual);

					if(*gpsqual == '1' || *gpsqual == '2'){
						sprintf(gpsjam,"%c%c",gpstimeraw[0],gpstimeraw[1]);
						sprintf(gpsmenit,"%c%c",gpstimeraw[2],gpstimeraw[3]);
						sprintf(gpsdetik,"%c%c",gpstimeraw[4],gpstimeraw[5]);
						gpsalt = atof((char*)gpsaltraw);
						gpssat = atoi((char*)gpssatraw);
						if(*gpslathead == 'N'){
						gpslat = atof((char*)gpslatraw);
						}
						else if(*gpslathead == 'S'){
						gpslat = atof((char*)gpslatraw) * -1;
						}
						if(*gpslonghead == 'E'){
						gpslong = atof((char*)gpslongraw);
						}
						else if(*gpslonghead == 'W'){
						gpslong = atof((char*)gpslongraw) * -1;
						}
					}
					else{
						gpslong = 0.0000;
						gpslat = 0.0000;
						gpssat = 0;
						gpsalt = 0.0;
						sprintf(gpsjam,"00");
						sprintf(gpsmenit,"00");
						sprintf(gpsdetik,"00");
					}
				}
			}
			i_=0;
			cmd_ = 0;
			clearchararray(datagpslkp, 100);
		}
	}
}

void gpsinit()
{
	HAL_UART_Receive_DMA(&huart1, (uint8_t*)rxgpsdata, 1);
}
