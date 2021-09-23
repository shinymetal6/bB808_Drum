/*
 * bB808.c
 *
 *  Created on: Sep 21, 2021
 *      Author: fil
 */

#include "main.h"
#include "menus.h"
#include "usb_host.h"
#include "usbh_def.h"
#include "usbh_MIDI.h"
#include "ff.h"

SystemVar_TypeDef	SystemVar;
extern	TIM_HandleTypeDef htim2;
extern	TIM_HandleTypeDef htim6;

void tim50msec_callback(void)
{
	SystemVar.tim50msec_flag = 1;
	SystemVar.tim100msec_flag ++;
	SystemVar.tim100msec_flag &= 1;
}

void InitLCD(char *title)
{
	  BSP_LCD_Init();
	  BSP_LCD_Clear(LCD_COLOR_BLACK);
	  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	  BSP_LCD_SetFont(&Font24);
	  BSP_LCD_DisplayStringAt(16, 1, (uint8_t *)title, LEFT_MODE);
	  BSP_LCD_SetFont(&Font12);
}

void bB808_Init(void)
{
	HAL_GPIO_WritePin(USB_OTGHS_PPWR_EN_GPIO_Port, USB_OTGHS_PPWR_EN_Pin, GPIO_PIN_SET);
	InitLCD("bB808");
	MenuDisplayInit();
	SystemVar.audio_init_flag = 0;
	HAL_TIM_Encoder_Start_IT(&htim2, TIM_CHANNEL_ALL);
	HAL_TIM_Base_Start_IT(&htim6);
	SystemVar.beat = 120;
	SystemVar.system = SYSTEM_MENU_INCDEC;
	DrawBPM(0);
	DrawDelay(0);
	DrawBPM_Icon();
	DelayLineInit();
	QSPISamplePlayerInit();
	QSPISamplePlayerStart();
}

void bB808_Loop(void)
{
	if ( SystemVar.tim50msec_flag == 1)
	{
		SystemVar.tim50msec_flag = 0;
		SystemVar.DrawBitIndicator_counter++;
		if (SystemVar.DrawBitIndicator_counter > 5 )
			DrawBitIndicator(1);
		else
			DrawBitIndicator(0);
		if (SystemVar.DrawBitIndicator_counter > 9 )
			SystemVar.DrawBitIndicator_counter = 0;
		if ( SystemVar.encoder_flag == 1 )
		{
			__disable_irq();
			if (( SystemVar.system & SYSTEM_MENU_INCDEC) == SYSTEM_MENU_INCDEC)
				MenuEncoderNavigate();
			else
			{
				if (( SystemVar.system & SYSTEM_BPM_INCDEC) == SYSTEM_BPM_INCDEC)
					BPM_IncDec();
				if (( SystemVar.system & SYSTEM_DELAYVAL_INCDEC) == SYSTEM_DELAYVAL_INCDEC)
					Delay_IncDec();
			}
			__enable_irq();
			SystemVar.encoder_flag = 0;
		}
		if ( SystemVar.sw_disable == 1 )
		{
			if ( HAL_GPIO_ReadPin(ENCODER_SW_GPIO_Port, ENCODER_SW_Pin) == 1 )
				SystemVar.sw_disable = 0;
		}
		else
		{
			if ( HAL_GPIO_ReadPin(ENCODER_SW_GPIO_Port, ENCODER_SW_Pin) == 0 )
			{
				__disable_irq();
				if (( SystemVar.system & SYSTEM_MENU_INCDEC) == SYSTEM_MENU_INCDEC)
					MeuEncoderChangeMenu();
				else
				{
					if (( SystemVar.system & SYSTEM_BPM_INCDEC) == SYSTEM_BPM_INCDEC)
					{
						SystemVar.system &= ~SYSTEM_BPM_INCDEC;
						SystemVar.system |= SYSTEM_MENU_INCDEC;
						DrawBPM(0);
					}
					if (( SystemVar.system & SYSTEM_DELAYVAL_INCDEC) == SYSTEM_DELAYVAL_INCDEC)
					{
						SystemVar.system &= ~SYSTEM_DELAYVAL_INCDEC;
						SystemVar.system |= SYSTEM_MENU_INCDEC;
						DrawDelay(0);
					}
				}
				__enable_irq();
				SystemVar.sw_disable = 1;
			}
		}
	}
	if ( SystemVar.tim100msec_flag == 1)
	{
		SystemVar.tim100msec_flag = 0;
	}
	if ( (SystemVar.system & SYSTEM_MIDIDEV_FLAG ) == SYSTEM_MIDIDEV_FLAG )
	{
		QSPI_Process_MIDI();
		SystemVar.system &= ~SYSTEM_MIDIDEV_FLAG;
	}
}

#define	FROM_FS		0
#define	FROM_HS		1
char USBDISKPath[4];          /* USB Host logical drive path */
FATFS USBDISKFatFs;           /* File system object for USB disk logical drive */
extern	const uint8_t usbdrive[];

void MSC_Application(uint8_t from)
{
	if(f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 0) != FR_OK)
		return;
	SystemVar.usbdisk_ready = 1;
	BSP_LCD_DrawBitmap(MENU_USBKEY_ICON_X, MENU_USBKEY_ICON_Y, (uint8_t *)usbdrive);
}

void USB_CallFromHS(USBH_HandleTypeDef *phost, uint8_t id)
{
	if ( strncmp((char *)phost->pClass[0]->Name,"MSC",3) == 0 )
		MSC_Application(FROM_HS);
}

void USB_DisconnectFromHS(USBH_HandleTypeDef *phost, uint8_t id)
{

	if ( SystemVar.usbdisk_ready == 1)
	{
		BSP_LCD_SetTextColor(MENU_DELETE_COLOR);
		BSP_LCD_FillRect(MENU_USBKEY_ICON_X, MENU_USBKEY_ICON_Y, MENU_USBKEY_ICON_W, MENU_USBKEY_ICON_H);
		SystemVar.usbdisk_ready = 0;
	}
}

