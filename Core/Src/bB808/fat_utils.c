/*
 * fat_utils.c
 *
 *  Created on: Sep 21, 2021
 *      Author: fil
 */

#include "main.h"
#include "bB808.h"
#include "ff.h"

extern	USBH_HandleTypeDef hUSBHost;
uint8_t files_on_disk[256][16];

FRESULT Explore_Disk(char *path)
{
FRESULT res = FR_OK;
FILINFO fno;
DIR dir;
uint8_t line_idx = 95 , file_idx=0;

	if (( SystemVar.system & SYSTEM_USB_INIT ) == SYSTEM_USB_INIT)
	{
		res = f_opendir(&dir, path);
		while (res == FR_OK)
		{
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0)
			{
				return FR_NO_PATH;
			}
			if (fno.fname[0] != '.')
			{
				strcpy((char *)files_on_disk[file_idx], fno.fname);
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_DisplayStringAt(0,line_idx, files_on_disk[file_idx], LEFT_MODE);
				line_idx += 15;
				file_idx++;

				if (( line_idx > 190 ) || (file_idx > 254 ))
					return FR_OK;
			}
		}
		return FR_OK;
	}
	return FR_DISK_ERR;
}

uint8_t	line[128];
FIL		ConfFile;

void ReadDescriptorFileFromUSB(uint8_t line_idx)
{
uint32_t	i;
int			instrument_number,midi_key;
uint8_t 	filename[16],len;
uint8_t		file_idx=0;

	if (( SystemVar.system & SYSTEM_USB_INIT ) != SYSTEM_USB_INIT)
		return;
	if(f_open(&ConfFile, "bB808.txt", FA_OPEN_EXISTING | FA_READ) == FR_OK)
	{
		for(i=0;i<NUM_INSTRUMENT;i++)
		{
			f_gets((char * )line,128,&ConfFile);
			if ( (len = strlen((char * )line)) != 0 )
			{
				if ( sscanf((char * )line,"%d %s %d",&instrument_number,filename,&midi_key) == 3 )
				{
					sprintf((char *)files_on_disk[file_idx],"%d %s %d",instrument_number,filename,midi_key);
					BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
					BSP_LCD_DisplayStringAt(0,line_idx, files_on_disk[file_idx], LEFT_MODE);
					line_idx += 15;
					file_idx++;
				}
			}
		}
		f_close(&ConfFile);
	}
}

void ClearDescriptorFileArea(uint8_t line_from)
{
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(0, line_from, LCD_RESOLUTION_X, LCD_RESOLUTION_Y-DIGIT_H-line_from);
}


