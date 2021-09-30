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
extern	TIM_HandleTypeDef htim7;

extern	const uint8_t usbdrive_not_present[];
extern	const uint8_t usbdrive_present[];

uint8_t	view_erase_state = 0;
void tim100msec_callback(void)
{
	SystemVar.timers_flag |= TIMER_100MS_FLAG;
	SystemVar.tim100msec_counter++;
	if ( SystemVar.tim100msec_counter > 9 )
	{
		SystemVar.tim100msec_counter = 0;
		SystemVar.timers_flag |= TIMER_1SEC_FLAG;
		if (( SystemVar.system & SYSTEM_ERASE_IN_PROGRESS) == SYSTEM_ERASE_IN_PROGRESS)
		{
			if ( view_erase_state == 0 )
				BSP_LCD_SetTextColor(MENU_DELETE_COLOR);
			else
				BSP_LCD_SetTextColor(LCD_COLOR_RED);

			BSP_LCD_DisplayStringAt(0, DLY_TITLE_TEXT_Y-40, (uint8_t *)"Erase in progress", CENTER_MODE);
			view_erase_state++;
			view_erase_state &= 1;
		}
	}
}

void encoder_rotation_callback(void)
{
	SystemVar.last_encval = SystemVar.encval;
	SystemVar.encval = TIM2->CNT;
	if ( SystemVar.last_encval != SystemVar.encval)
		SystemVar.encoder_flag |= ENCODER_ROTATION_FLAG;
}

void encoder_sw_callback(void)
{
	if ( HAL_GPIO_ReadPin(ENCODER_SW_GPIO_Port, ENCODER_SW_Pin) == 0 )
		SystemVar.encoder_flag |= ENCODER_SW_FLAG;
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
	BSP_LCD_DrawBitmap(MENU_USBKEY_ICON_X, MENU_USBKEY_ICON_Y, (uint8_t *)usbdrive_not_present);
}

static void InitialSetup(void)
{
	SystemVar.beat = 120;
	SystemVar.delay_weight_int = 50;
	SystemVar.system = SYSTEM_MENU_INCDEC;
}

void bB808_Init(void)
{
	HAL_GPIO_WritePin(USB_OTGHS_PPWR_EN_GPIO_Port, USB_OTGHS_PPWR_EN_Pin, GPIO_PIN_SET);
	InitLCD("bB808");
	InitialSetup();
	MenuDisplayInit();
	DelayLineInit();
	BPM_Init();
	Delay_Weight_Draw(0);
	BPM_Draw(0);
	Delay_Draw(0);
	DelayTypeDisplay();
	QSPISamplePlayerInit();
	SequencerInit();
	HAL_TIM_Encoder_Start_IT(&htim2, TIM_CHANNEL_ALL);
	HAL_TIM_Base_Start_IT(&htim6);	/* tim @ 50 mSec */
	HAL_TIM_Base_Start_IT(&htim7);	/* sequencer @ 10 mSec */
	QSPISamplePlayerStart();
}

void bB808_Loop(void)
{
	if (( SystemVar.timers_flag & TIMER_100MS_FLAG ) == TIMER_100MS_FLAG)
	{
		SystemVar.timers_flag &= ~TIMER_100MS_FLAG;
		if (( SystemVar.encoder_flag & ENCODER_ROTATION_FLAG) == ENCODER_ROTATION_FLAG)
		{
			if (( SystemVar.system & SYSTEM_MENU_INCDEC) == SYSTEM_MENU_INCDEC)
				MenuEncoderNavigate();
			else
			{
				if (( SystemVar.system & SYSTEM_BPM_INCDEC) == SYSTEM_BPM_INCDEC)
					BPM_IncDec();
				if (( SystemVar.system & SYSTEM_DELAYVAL_INCDEC) == SYSTEM_DELAYVAL_INCDEC)
					Delay_IncDec();
				if (( SystemVar.system & SYSTEM_DELAYWEIGHT_INCDEC) == SYSTEM_DELAYWEIGHT_INCDEC)
					Delay_Weight_IncDec();
			}
			SystemVar.encoder_flag &= ~ENCODER_ROTATION_FLAG;
		}
		if (( SystemVar.encoder_flag & ENCODER_SW_FLAG) == 0)
		{
			if ( HAL_GPIO_ReadPin(ENCODER_SW_GPIO_Port, ENCODER_SW_Pin) == 0 )
			{
				if (( SystemVar.system & SYSTEM_MENU_INCDEC) == SYSTEM_MENU_INCDEC)
					MeuEncoderChangeMenu();
				else
				{
					if (( SystemVar.system & SYSTEM_BPM_INCDEC) == SYSTEM_BPM_INCDEC)
					{
						SystemVar.system &= ~SYSTEM_BPM_INCDEC;
						SystemVar.system |= SYSTEM_MENU_INCDEC;
						BPM_Draw(0);
					}
					if (( SystemVar.system & SYSTEM_DELAYVAL_INCDEC) == SYSTEM_DELAYVAL_INCDEC)
					{
						SystemVar.system &= ~SYSTEM_DELAYVAL_INCDEC;
						SystemVar.system |= SYSTEM_MENU_INCDEC;
						Delay_Draw(0);
					}
					if (( SystemVar.system & SYSTEM_DELAYWEIGHT_INCDEC) == SYSTEM_DELAYWEIGHT_INCDEC)
					{
						SystemVar.system &= ~SYSTEM_DELAYWEIGHT_INCDEC;
						SystemVar.system |= SYSTEM_MENU_INCDEC;
						Delay_Weight_Draw(0);
					}
				}
				SystemVar.encoder_flag |= ENCODER_SW_FLAG;
			}
		}
		else
		{
			if ( HAL_GPIO_ReadPin(ENCODER_SW_GPIO_Port, ENCODER_SW_Pin) == 1 )
				SystemVar.encoder_flag &= ~ENCODER_SW_FLAG;
		}
		if ( HAL_GPIO_ReadPin(SINGLE_SEQ_GPIO_Port, SINGLE_SEQ_Pin) == 1 )
		{
			SystemVar.sequencer |= SEQUENCER_SINGLE;
		}

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


void MSC_Application(uint8_t from)
{
	if(f_mount(&USBDISKFatFs, (TCHAR const*)USBDISKPath, 0) != FR_OK)
		return;
	SystemVar.system |= SYSTEM_USB_INIT;
	BSP_LCD_DrawBitmap(MENU_USBKEY_ICON_X, MENU_USBKEY_ICON_Y, (uint8_t *)usbdrive_present);
}

void USB_CallFromHS(USBH_HandleTypeDef *phost, uint8_t id)
{
	if ( strncmp((char *)phost->pClass[0]->Name,"MSC",3) == 0 )
		MSC_Application(FROM_HS);
}

void USB_DisconnectFromHS(USBH_HandleTypeDef *phost, uint8_t id)
{

	if (( SystemVar.system & SYSTEM_USB_INIT) == SYSTEM_USB_INIT)
	{
		BSP_LCD_SetTextColor(MENU_DELETE_COLOR);
		BSP_LCD_FillRect(MENU_USBKEY_ICON_X, MENU_USBKEY_ICON_Y, MENU_USBKEY_ICON_W, MENU_USBKEY_ICON_H);
		SystemVar.system &= ~SYSTEM_USB_INIT;
	}
}

