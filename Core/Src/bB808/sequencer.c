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
	SystemVar.sequencer_length = sequencer_steps[0];
	SystemVar.sequencer_counter = 0;
	SystemVar.sequencer_preload = ((MAX_BEAT-SystemVar.beat) * 100 ) / 60;

	SystemVar.sequencer_length = 5;
	sequencer_steps[0] = 5;
	sequencer_steps[1] = 0x1;
	sequencer_steps[2] = 0x1;
	sequencer_steps[3] = 0x1;
	sequencer_steps[4] = 0x1;
}

void Sequencer(void)
{
uint16_t	i , val;
	if ( (SystemVar.system & SYSTEM_INTEXT_SEQUENCER ) == SYSTEM_INTEXT_SEQUENCER )
	{
		SystemVar.sequencer_counter++;
		if ( SystemVar.sequencer_counter >= SystemVar.sequencer_preload )
		{
			SystemVar.sequencer_counter = 0;
			SystemVar.sequencer |= SEQUENCER_TICK;
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
				SystemVar.sequencer_step = 1;
		}
	}
	else
		SystemVar.sequencer_counter = 0;
}

/* 10 mSec sequencer callback */
void SequencerCallback(void)
{
	SystemVar.sequencer |= SEQUENCER_IRQ_FLAG;
	Sequencer();
}

void SequencerSet(void)
{
	SystemVar.sequencer_preload = ((MAX_BEAT-SystemVar.beat) * 100 ) / 60;
}
