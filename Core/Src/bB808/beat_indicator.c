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
