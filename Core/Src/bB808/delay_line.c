/*
 * delay_line.c
 *
 *  Created on: Jul 1, 2021
 *      Author: fil
 */

#include "main.h"
#include "delay_line.h"

__attribute__ ((aligned (16))) int16_t	delay_line[DELAY_LINE_SIZE];
uint32_t	delay_insertion_pointer=0,delay_extraction_pointer=DELAY_LINE_SIZE/2;
float		delay_weight	= 0.5F;

//#define	USE_PSRAM	1

int16_t DelayLine(int16_t sample , uint8_t delay_type)
{
int16_t		out_sample ;

#ifdef USE_PSRAM
uint16_t	psram_sample;
	BSP_PSRAM_ReadData(PSRAM_DEVICE_ADDR + delay_extraction_pointer, &psram_sample, 1);
	out_sample = (int16_t )((float )sample * (1.0F - delay_weight)) + ((float )psram_sample * delay_weight);
#else
	out_sample = (int16_t )((float )sample * (1.0F - delay_weight)) + ((float )delay_line[delay_extraction_pointer] * delay_weight);
#endif
	if ( delay_type == DELAY_TYPE_FLANGER)
#ifdef USE_PSRAM
		BSP_PSRAM_WriteData(PSRAM_DEVICE_ADDR + delay_insertion_pointer, (uint16_t *)&sample, 1);
#else
		delay_line[delay_insertion_pointer] = sample;
#endif
	else
#ifdef USE_PSRAM
		BSP_PSRAM_WriteData(PSRAM_DEVICE_ADDR + delay_insertion_pointer, (uint16_t *)&out_sample, 1);
#else
		delay_line[delay_insertion_pointer] = out_sample;
#endif

	delay_extraction_pointer++;
	delay_insertion_pointer++;
	delay_extraction_pointer &= (DELAY_LINE_SIZE-1);
	delay_insertion_pointer  &= (DELAY_LINE_SIZE-1);
	return out_sample;
}

#define	CLEARBUF_SIZE	256
static uint16_t	clearpsrambuf[CLEARBUF_SIZE];
void DelayLineInit(void)
{
uint16_t	i,k=0;
	BSP_PSRAM_Init();
	for(i=0;i<CLEARBUF_SIZE;i++)
		clearpsrambuf[i] = 0;
	for(i=0;i<DELAY_LINE_SIZE/CLEARBUF_SIZE;i++,k+=CLEARBUF_SIZE)
		BSP_PSRAM_WriteData(PSRAM_DEVICE_ADDR + i*CLEARBUF_SIZE, clearpsrambuf, CLEARBUF_SIZE);
}


