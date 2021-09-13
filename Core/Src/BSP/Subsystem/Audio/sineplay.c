/*
 * sineplay.c
 *
 *  Created on: Aug 12, 2021
 *      Author: fil
 */


#include "main.h"
#include "sineplay.h"
#include <stdio.h>

#ifdef SINEPLAYER
__attribute__ ((aligned (16)))  uint16_t sine_buf[AUDIO_OUT_BUFFER_SIZE_INT16];

AUDIO_BufferTypeDef  	 SinePlay;
AUDIO_OUT_BufferTypeDef  BufferCtl;

void InitSinePlay(void)
{
	SinePlay.Volume = SINEPLAY_DEFAULT_VOLUME;
	SinePlay.AudioFreq = SINEPLAY_DEFAULT_SAMPLEFREQ;
	BSP_LCD_SetFont(&Font12);
	if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, SinePlay.Volume, SinePlay.AudioFreq) == 0)
	{
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_DisplayStringAt(0, 40, (uint8_t *)"  AUDIO CODEC OK  ", LEFT_MODE);
	}
	else
	{
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		BSP_LCD_DisplayStringAt(0, 40, (uint8_t *)" AUDIO CODEC FAIL ", LEFT_MODE);
		BSP_LCD_DisplayStringAt(0, 50, (uint8_t *)" Try to reset board ", LEFT_MODE);
	}

	BSP_AUDIO_OUT_Play(sine_buf, AUDIO_OUT_BUFFER_SIZE_INT16);
    BSP_AUDIO_OUT_SetMute(AUDIO_MUTE_OFF);
}

uint8_t SinePlay_Process(void)
{
	if (( BufferCtl.flag &  SINEPLAY_STATE_FLAG_HALF ) == SINEPLAY_STATE_FLAG_HALF)
	{
		int i;
		for(i=0;i<AUDIO_OUT_BUFFER_SIZE_HALF_INT16;i++)
			sine_buf[i] = audio_sample[i];
		BufferCtl.flag &= ~SINEPLAY_STATE_FLAG_HALF;
	}

	if (( BufferCtl.flag &  SINEPLAY_STATE_FLAG_FULL ) == SINEPLAY_STATE_FLAG_FULL)
	{
		int i;
		for(i=AUDIO_OUT_BUFFER_SIZE_HALF_INT16;i<AUDIO_OUT_BUFFER_SIZE_INT16;i++)
			sine_buf[i] = audio_sample[i];
		BufferCtl.flag &= ~SINEPLAY_STATE_FLAG_FULL;
	}

    return 0;
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
	/* allows AUDIO_Process() to refill 2nd part of the buffer  */
	BufferCtl.flag |= SINEPLAY_STATE_FLAG_FULL;
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
	/* allows AUDIO_Process() to refill 1st part of the buffer  */
	BufferCtl.flag |= SINEPLAY_STATE_FLAG_HALF;
}

#endif
