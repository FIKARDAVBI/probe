/*
 * kom.c
 *
 *  Created on: Dec 11, 2022
 *      Author: user
 */
#include "kom.h"
#include "task.h"
#include "stm32f4xx_hal.h"

char cx[]="CX",st[]="ST",sim[]="SIM",simp[]="SIMP",cal[]="CAL",bk[]="BK",tc[]="TC",cr[]="CR", end[]="\n\r",tn[]="1085";
uint8_t cmd,rxdata[2],datalkp[30],i;
extern UART_HandleTypeDef huart2;

void isidata(uint8_t urutan,char dat_[])
{
	for(int c=0;c<28;c++)
		dat_[c]=0;
	uint8_t n=0,k=0,m=0;
	while(n<urutan)
	{
		if(datalkp[m]=='\r') n=urutan;
		if(datalkp[m]==',') n++;
		if(n == urutan-1 && datalkp[m] != ',')
		{
			dat_[k]=(char)datalkp[m];
			k++;
		}
		m++;
	}
}

uint8_t cocokan(uint8_t urutan,char dat[])
{
	char buf[30];
	uint8_t hasil=0;
	isidata(urutan,buf);
	uint8_t k=0;

	while(buf[k] != '\0')
	{
		k++;
		if(buf[k]==dat[k])hasil++;
	}
	return hasil;
}

void checkdata_()
{
	HAL_UART_Receive_DMA(&huart2, (uint8_t*)rxdata, 1);
	if(rxdata[0]=='C')
	{
		cmd++;
		datalkp[i]=rxdata[0];
		i++;
	}
	else if(cmd>0)
	{
		if(rxdata[0] == ',')
		{
			datalkp[i]=',';
			i++;
			cmd++;
		}
		else if(rxdata[0] == '\r')
		{
			datalkp[i]='\r';
			i++;
			cmd = 0;
			i=0;

			if((cocokan(3,cx)==2)&&(cocokan(2,tn)==4))CX();
			else if ((cocokan(3,sim)==3)&&(cocokan(2,tn)==4)) SIM();
			else if ((cocokan(3,simp)==4)&&(cocokan(2,tn)==4)) SIMP();
			else if ((cocokan(3,cal)==3)&&(cocokan(2,tn)==4)) CAL();
			else if ((cocokan(3,st)==2)&&(cocokan(2,tn)==4)) ST();
			else if ((cocokan(3,bk)==2)&&(cocokan(2,tn)==4)) BK();
			else if ((cocokan(3,cr)==2)&&(cocokan(2,tn)==4)) CR();
			else if ((cocokan(3,tc)==2)&&(cocokan(2,tn)==4)) TK();
		}
		else
		{
			datalkp[i]=rxdata[0];
			i++;
			cmd++;
		}
	}
}

void kominit(void)
{
	HAL_UART_Receive_DMA(&huart2, (uint8_t*)rxdata, 1);
}
