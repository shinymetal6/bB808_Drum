/*
 * LCD_Log.c
 *
 *  Created on: Jul 27, 2021
 *      Author: fil
 */
#include "main.h"

#define LOG_BUFFER_SIZE 256
/*
int LCD_UsrVarLog(const char* format, ...)
{
char buf[LOG_BUFFER_SIZE];
va_list argptr;
	va_start(argptr, format);
	int cnt = vsnprintf(buf, LOG_BUFFER_SIZE, format, argptr);
	va_end(argptr);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(0, 70, (uint8_t *)buf, CENTER_MODE);
    return cnt;
}
*/
uint16_t	line = 80;
int LCD_UsrLog(char* buf)
{
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(0, line, (uint8_t *)buf, CENTER_MODE);
	line += 10;
    return 0;
}

