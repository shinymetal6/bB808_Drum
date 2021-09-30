/*
 * sequencer.c
 *
 *  Created on: Sep 24, 2021
 *      Author: fil
 */

#include "main.h"
#include "bB808.h"

__attribute__ ((aligned (16)))  uint16_t sequencer_steps[SEQUENCER_MAX_SIZE];
extern	Instrument_TypeDef  Instrument;
uint32_t	bpm2irq[MAX_BEAT];

void SequencerInit(void)
{
	BSP_QSPI_Read((uint8_t *)sequencer_steps, QSPI_SEQUENCER_ADDRESS, SEQUENCER_MAX_SIZE);
	if (( sequencer_steps[0] == (('s'<<8) | 'e')) && ( sequencer_steps[1] == (('q'<<8) | '8')))
		SystemVar.sequencer_length = sequencer_steps[2];
	else
		SystemVar.sequencer_length = 0;

	SystemVar.sequencer_counter = 0;
}

void SequencerCallback(void)
{
uint16_t	i , val;

	if ((((SystemVar.system & SYSTEM_INTEXT_SEQUENCER ) == SYSTEM_INTEXT_SEQUENCER ) && (SystemVar.sequencer_length != 0 )) |
			((SystemVar.sequencer & SEQUENCER_SINGLE ) == SEQUENCER_SINGLE ))
	{
		for(i=0;i<NUM_INSTRUMENT;i++)
		{
			val = 1 << i;
			if (( sequencer_steps[SystemVar.sequencer_step] & val) != 0 )
			{
				Instrument.qspi_ptr[i] = HEADER_SIZE;
				Instrument.sample_active_flag |= (1 << i);
			}
		}
		SystemVar.sequencer_step ++;
		if ( SystemVar.sequencer_step > SystemVar.sequencer_length )
		{
			SystemVar.sequencer_step = 1;
			if ( (SystemVar.sequencer & SEQUENCER_SINGLE ) == SEQUENCER_SINGLE )
			{
				SystemVar.sequencer &= ~SEQUENCER_SINGLE;
			}
		}
		if ((SystemVar.sequencer & SEQUENCER_PRELOAD ) == SEQUENCER_PRELOAD)
		{
			TIM7->ARR = SystemVar.sequencer_preload;
			SystemVar.sequencer &= ~SEQUENCER_PRELOAD;
		}
	}
	else
		SystemVar.sequencer_step = 1;

	SystemVar.sequencer |= SEQUENCER_IRQ_FLAG;
}

