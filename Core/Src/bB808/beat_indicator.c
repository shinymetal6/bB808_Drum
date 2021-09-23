/*
 * beat_indicator.c
 *
 *  Created on: Sep 18, 2021
 *      Author: fil
 */

#include "main.h"
#include "beat_indicator.h"

extern	LCD_DrawPropTypeDef DrawProp;

void DrawBitIndicator(uint8_t on)
{
uint32_t Color = DrawProp.TextColor;
	if ( on == 1 )
		DrawProp.TextColor = LCD_COLOR_RED;
	else
		DrawProp.TextColor = LCD_COLOR_BLACK;
	BSP_LCD_FillCircle(8, 8, 4);
	DrawProp.TextColor = Color;
}

void DrawBPM(uint8_t hilight)
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

void DrawBPM_Icon(void)
{
	BSP_LCD_DrawBitmap(MENU_BEAT_ICON_X, MENU_BEAT_ICON_Y, (uint8_t *)beat);
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
	DrawBPM(1);
}
