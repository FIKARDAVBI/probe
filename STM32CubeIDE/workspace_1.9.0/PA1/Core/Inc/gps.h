/*
 * gps.h
 *
 *  Created on: Mar 6, 2023
 *      Author: user
 */

#ifndef INC_GPS_H_
#define INC_GPS_H_
#include "stm32f4xx_hal.h"

void checkgpsdata();
void gpsinit();
void parsedata(uint8_t urutan_ ,uint8_t data[]);
uint8_t checkNMEAGPGGA(uint8_t* data);
void clearchararray(uint8_t* data_, int len);

#endif /* INC_GPS_H_ */
