/*
 * delay_line.c
 *
 *  Created on: Jul 1, 2021
 *      Author: fil
 */

#include "main.h"
#include "bB808.h"

__attribute__ ((aligned (16))) int16_t	delay_line[DELAY_LINE_SIZE];

void DelayLineInit(void)
{
	SystemVar.delay_insertion_pointer = 0;
	SystemVar.delay = 1;
	SystemVar.delay_weight = 0.5F;
	SystemVar.delay_type = 0;
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_DisplayStringAt(DLY_TEXT_X,DLY_TEXT_Y, (uint8_t *)"None", LEFT_MODE);
	BSP_LCD_SetFont(&Font12);
}

void DelayTypeDisplay(void)
{
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font16);
	if (( SystemVar.delay_type & DELAY_TYPE_FLANGER) == DELAY_TYPE_FLANGER)
		BSP_LCD_DisplayStringAt(DLY_TEXT_X,DLY_TEXT_Y, (uint8_t *)"REV ", LEFT_MODE);
	else if (( SystemVar.delay_type & DELAY_TYPE_ECHO) == DELAY_TYPE_ECHO )
		BSP_LCD_DisplayStringAt(DLY_TEXT_X,DLY_TEXT_Y, (uint8_t *)"ECHO", LEFT_MODE);
	else
		BSP_LCD_DisplayStringAt(DLY_TEXT_X,DLY_TEXT_Y, (uint8_t *)"None", LEFT_MODE);
	BSP_LCD_SetFont(&Font12);
}

int16_t DelayLine(int16_t sample , uint8_t delay_type)
{
int16_t		out_sample = 0;
uint16_t	delay_extraction_pointer;

	delay_extraction_pointer = SystemVar.delay_insertion_pointer - SystemVar.delay*128;

	out_sample = (int16_t )((float )sample * (1.0F - SystemVar.delay_weight)) + ((float )delay_line[delay_extraction_pointer] * SystemVar.delay_weight);
	if (( SystemVar.delay_type & DELAY_TYPE_FLANGER) == DELAY_TYPE_FLANGER)
		delay_line[SystemVar.delay_insertion_pointer] = sample;
	else if (( SystemVar.delay_type & DELAY_TYPE_ECHO) == DELAY_TYPE_ECHO )
		delay_line[SystemVar.delay_insertion_pointer] = out_sample;
	else
		out_sample = sample;
	SystemVar.delay_insertion_pointer++;
	return out_sample;
}

void Delay_Draw(uint8_t hilight)
{
uint32_t 	h,t,u;  // hundreds,tens,units

	u = SystemVar.delay%10;
	t = (SystemVar.delay/10)%10;
	h = (SystemVar.delay/100)%10;

	if ( hilight == 0 )
	{
		BSP_LCD_DrawBitmap(DLY_DIGIT_XPOSH, DLY_DIGIT_YPOS, (uint8_t *)green_digits[h]);
		BSP_LCD_DrawBitmap(DLY_DIGIT_XPOST, DLY_DIGIT_YPOS, (uint8_t *)green_digits[t]);
		BSP_LCD_DrawBitmap(DLY_DIGIT_XPOSU, DLY_DIGIT_YPOS, (uint8_t *)green_digits[u]);
	}
	else
	{
		BSP_LCD_DrawBitmap(DLY_DIGIT_XPOSH, DLY_DIGIT_YPOS, (uint8_t *)red_digits[h]);
		BSP_LCD_DrawBitmap(DLY_DIGIT_XPOST, DLY_DIGIT_YPOS, (uint8_t *)red_digits[t]);
		BSP_LCD_DrawBitmap(DLY_DIGIT_XPOSU, DLY_DIGIT_YPOS, (uint8_t *)red_digits[u]);
	}
}

void Delay_IncDec(void)
{
	if ( SystemVar.encval > SystemVar.last_encval )
	{
		if ( SystemVar.delay != 0 )
			SystemVar.delay--;
	}
	else
	{
		SystemVar.delay++;
		if ( SystemVar.delay > MAX_DELAY )
			SystemVar.delay = MAX_DELAY;
	}
	Delay_Draw(1);
}
