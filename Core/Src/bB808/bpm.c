/*
 * beat_indicator.c
 *
 *  Created on: Sep 18, 2021
 *      Author: fil
 */

#include "main.h"
#include "bpm.h"

extern	LCD_DrawPropTypeDef DrawProp;

void BPM_Draw(uint8_t hilight)
{
uint32_t 	h,t,u;  // hundreds,tens,units

	u = SystemVar.beat%10;
	t = (SystemVar.beat/10)%10;
	h = (SystemVar.beat/100)%10;
	if ( hilight == 0 )
	{
		BSP_LCD_DrawBitmap(BPM_DIGIT_XPOSH, BPM_DIGIT_YPOS, (uint8_t *)blue_digits[h]);
		BSP_LCD_DrawBitmap(BPM_DIGIT_XPOST, BPM_DIGIT_YPOS, (uint8_t *)blue_digits[t]);
		BSP_LCD_DrawBitmap(BPM_DIGIT_XPOSU, BPM_DIGIT_YPOS, (uint8_t *)blue_digits[u]);
	}
	else
	{
		BSP_LCD_DrawBitmap(BPM_DIGIT_XPOSH, BPM_DIGIT_YPOS, (uint8_t *)red_digits[h]);
		BSP_LCD_DrawBitmap(BPM_DIGIT_XPOST, BPM_DIGIT_YPOS, (uint8_t *)red_digits[t]);
		BSP_LCD_DrawBitmap(BPM_DIGIT_XPOSU, BPM_DIGIT_YPOS, (uint8_t *)red_digits[u]);
	}
}


void BPM_IncDec(void)
{
	if ( SystemVar.encval > SystemVar.last_encval )
	{
		SystemVar.beat--;
		if ( SystemVar.beat == 0 )
			SystemVar.beat = 1;
	}
	else
	{
		SystemVar.beat++;
		if ( SystemVar.beat > MAX_BEAT )
			SystemVar.beat = MAX_BEAT;
	}
	BPM_Draw(1);
	SequencerSet();
}

void BPM_Init(void)
{
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_DisplayStringAt(BPM_TEXT_X,BPM_TEXT_Y, (uint8_t *)"BPM", LEFT_MODE);
	BSP_LCD_SetFont(&Font12);
}
