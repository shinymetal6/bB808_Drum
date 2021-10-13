/*
 * sequencer.c
 *
 *  Created on: Sep 24, 2021
 *      Author: fil
 */

#include "main.h"
#include "bB808.h"
#include <string.h>

__attribute__ ((aligned (16)))  uint16_t sequencer_steps[SEQUENCER_MAX_SIZE];
__attribute__ ((aligned (16)))  Sequencer_DescriptorTypeDef	Sequencer_Descriptor;

extern	Instrument_TypeDef  Instrument;
uint32_t	bpm2irq[MAX_BEAT];

void SequencerInit(void)
{
	BSP_QSPI_Read((uint8_t *)&Sequencer_Descriptor, QSPI_SEQUENCER_ADDRESS, sizeof(Sequencer_Descriptor));
	if ( strncmp((char *)Sequencer_Descriptor.header,"Seq_8xx",HEADER_LEN-1) == 0 )
	{
		SystemVar.beat = Sequencer_Descriptor.sequencer_beat;
		SystemVar.sequencer_preload = BPM_1_BPM - (SystemVar.beat * BPM_UNIT);
		TIM7->ARR = SystemVar.sequencer_preload;
		SystemVar.delay_type = Sequencer_Descriptor.delay_type;
		SystemVar.delay = Sequencer_Descriptor.delay;
		SystemVar.delay_weight = (float )Sequencer_Descriptor.delay_weight / 100.0F;;
		SystemVar.sequencer_length = Sequencer_Descriptor.sequencer_length;
		SystemVar.sequencer_counter = 0;
		SystemVar.sequencer_step = 0;
		BPM_Draw(0);
		DelayTypeDisplay();
		Delay_Draw(0);
	}
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
			if (( Sequencer_Descriptor.sequencer_steps[SystemVar.sequencer_step] & val) != 0 )
			{
				Instrument.qspi_ptr[i] = HEADER_SIZE;
				Instrument.sample_active_flag |= (1 << i);
			}
		}
		SystemVar.sequencer_step ++;
		if ( SystemVar.sequencer_step >= SystemVar.sequencer_length )
		{
			SystemVar.sequencer_step = 0;
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
		SystemVar.sequencer_step = 0;

	SystemVar.sequencer |= SEQUENCER_IRQ_FLAG;
}

