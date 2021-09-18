/*
 * helpers.c
 *
 *  Created on: Jul 27, 2021
 *      Author: fil
 */

#include "main.h"
#include "usb_host.h"
#include "usbh_def.h"
#include "usbh_MIDI.h"

/* dekrispator */

#include "fatfs.h"

FATFS USBDISKFatFs;           /* File system object for USB disk logical drive */
FIL MyFile;                   /* File object */
char USBDISKPath[4];          /* USB Host logical drive path */

USBH_HandleTypeDef hUSBHost; /* USB Host handle */

extern	uint32_t	usbdisk_ready;

void InitLCD(char *title)
{
	  BSP_LCD_Init();
	  BSP_LCD_Clear(LCD_COLOR_BLACK);
	  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	  BSP_LCD_SetFont(&Font24);
	  BSP_LCD_DisplayStringAt(0, 1, (uint8_t *)title, CENTER_MODE);
	  BSP_LCD_SetFont(&Font12);
}

void USB_Error_Handler(char *text)
{
	  BSP_LCD_SetTextColor(LCD_COLOR_RED);
	  BSP_LCD_DisplayStringAt(0, 60, (uint8_t *)text, CENTER_MODE);
	  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
}

uint8_t	midi_buf[64],msg[64],midi_buffer[64];

void MSC_Application(uint8_t from)
{
#ifdef VERBOSE_USB_HELPERS
FRESULT res;                                          /* FatFs function common result code */
uint32_t  bytesread;                     /* File write/read counts */
uint8_t rtext[100];                                   /* File read buffer */
#endif
	if(f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 0) != FR_OK)
	{
		USB_Error_Handler("Error mounting disk");
		return;
	}
#ifdef VERBOSE_USB_HELPERS
	/*
	if(f_open(&MyFile, "Even.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	{
		USB_Error_Handler("Error creating file");
		return;
	}
	res = f_write(&MyFile, wtext, sizeof(wtext), (void *)&byteswritten);
	if((byteswritten == 0) || (res != FR_OK))
	{
		USB_Error_Handler("Error writing file");
		return;
	}
	f_close(&MyFile);
*/
	if(f_open(&MyFile, "sample.wav", FA_READ) != FR_OK)
	{
		USB_Error_Handler("Error opening file");
		return;
	}
	res = f_read(&MyFile, rtext, sizeof(rtext), (void *)&bytesread);
	if((bytesread == 0) || (res != FR_OK))
	{
		USB_Error_Handler("Error reading file");
		return;
	}

	f_close(&MyFile);
	BSP_LCD_DisplayStringAt(0, 70, (uint8_t *)"USB OK", CENTER_MODE);
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
#endif
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_DisplayStringAt(0, 20, (uint8_t *)"Ready", CENTER_MODE);
	usbdisk_ready = 1;

	/* Unlink the USB disk I/O driver */
	//FATFS_UnLinkDriver(USBDISKPath);
}

extern	void MIDI_Application(uint8_t from,USBH_HandleTypeDef *phost, uint8_t id);

__weak void USB_CallFromFS(USBH_HandleTypeDef *phost, uint8_t id)
{
	if ( strncmp((char *)phost->pClass[0]->Name,"MIDI",4) == 0 )
		MIDI_Application(0,phost,id);
}

__weak void USB_CallFromHS(USBH_HandleTypeDef *phost, uint8_t id)
{
	if ( strncmp((char *)phost->pClass[0]->Name,"MSC",3) == 0 )
		MSC_Application(1);
}

extern	uint8_t	tim1sec_flag;
void tim1sec_callback(void)
{
	tim1sec_flag = 1;
}
