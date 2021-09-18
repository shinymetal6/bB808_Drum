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


int16_t DelayLine(int16_t sample , uint8_t delay_type)
{
	/* delay value and delay_type are MIDI, so 0..127 */
//float		weight = ((float )delay_weight / 256.0F);	// max weight is 50%
int16_t		out_sample ;

	out_sample = (int16_t )((float )sample * (1.0F - delay_weight)) + ((float )delay_line[delay_extraction_pointer] * delay_weight);
	if ( delay_type == DELAY_TYPE_FLANGER)
		delay_line[delay_insertion_pointer] = sample;
	else
		delay_line[delay_insertion_pointer] = out_sample;

	delay_extraction_pointer++;
	delay_insertion_pointer++;
	delay_extraction_pointer &= (DELAY_LINE_SIZE-1);
	delay_insertion_pointer  &= (DELAY_LINE_SIZE-1);
	return out_sample;
}

