/*
 * menus.c
 *
 *  Created on: Sep 20, 2021
 *      Author: fil
 */

#include "main.h"
#include "bB808.h"


Menu_TypeDef	MenuTop[] =
{
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y,
			"Samples",
			MENU_ACTIVE_COLOR,
		},
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y+MENU_FONT_HEIGHT,
			"Sequence",
			MENU_INACTIVE_COLOR,
		},
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y+2*MENU_FONT_HEIGHT,
			"Echo - Reverb",
			MENU_INACTIVE_COLOR,
		},
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y+3*MENU_FONT_HEIGHT,
			"Global",
			MENU_INACTIVE_COLOR,
		},
		{
			0,0,
		},
};

Menu_TypeDef	MenuSamples[] =
{
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y,
			"View Samples on USB key",
			MENU_ACTIVE_COLOR,
		},
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y+MENU_FONT_HEIGHT,
			"View Samples on internal FLASH",
			MENU_INACTIVE_COLOR,
		},
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y+2*MENU_FONT_HEIGHT,
			"USB to Flash Sample Transfer",
			MENU_INACTIVE_COLOR,
		},
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y+4*MENU_FONT_HEIGHT,
			"Return",
			MENU_INACTIVE_COLOR,
		},
		{
			0,0,
		},
};

Menu_TypeDef	MenuSequence[] =
{
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y,
			"View Sequence on USB key",
			MENU_ACTIVE_COLOR,
		},
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y+MENU_FONT_HEIGHT,
			"View Sequence on internal FLASH",
			MENU_INACTIVE_COLOR,
		},
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y+2*MENU_FONT_HEIGHT,
			"USB to Flash Sequence Transfer",
			MENU_INACTIVE_COLOR,
		},
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y+3*MENU_FONT_HEIGHT,
			"Return",
			MENU_INACTIVE_COLOR,
		},
		{
			0,0,
		},
};

Menu_TypeDef	MenuDelay[] =
{
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y,
			"Set Delay",
			MENU_ACTIVE_COLOR,
		},
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y+MENU_FONT_HEIGHT,
			"Set Type",
			MENU_INACTIVE_COLOR,
		},
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y+2*MENU_FONT_HEIGHT,
			"Return",
			MENU_INACTIVE_COLOR,
		},
		{
			0,0,
		},
};

Menu_TypeDef	MenuSettings[] =
{
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y,
			"Internal Loop Mode",
			MENU_ACTIVE_COLOR,
		},
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y+MENU_FONT_HEIGHT,
			"External Sequencer mode",
			MENU_INACTIVE_COLOR,
		},
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y+2*MENU_FONT_HEIGHT,
			"BPM",
			MENU_INACTIVE_COLOR,
		},
		{
			MENU_LINE_0_X,
			MENU_LINE_0_Y+3*MENU_FONT_HEIGHT,
			"Return",
			MENU_INACTIVE_COLOR,
		},
		{
			0,0,
		},
};

void   encoder_callback(void)
{
	SystemVar.last_encval = SystemVar.encval;
	SystemVar.encoder_flag = 1;
	SystemVar.encval = TIM2->CNT;
}

void MenuHilightItem(Menu_TypeDef *menu)
{
	if ( SystemVar.next_menu_item > menu[0].items)
		SystemVar.next_menu_item = 0;

	menu[SystemVar.selected_menu_item].color = MENU_INACTIVE_COLOR;
	BSP_LCD_SetTextColor(menu[SystemVar.selected_menu_item].color);
	BSP_LCD_DisplayStringAt(menu[SystemVar.selected_menu_item].linex,menu[SystemVar.selected_menu_item].liney, menu[SystemVar.selected_menu_item].text, LEFT_MODE);

	menu[SystemVar.next_menu_item].color = MENU_ACTIVE_COLOR;
	BSP_LCD_SetTextColor(menu[SystemVar.next_menu_item].color);
	BSP_LCD_DisplayStringAt(menu[SystemVar.next_menu_item].linex,menu[SystemVar.next_menu_item].liney, menu[SystemVar.next_menu_item].text, LEFT_MODE);

	SystemVar.selected_menu_item = SystemVar.next_menu_item;
}

void MenuDisplayMenu(Menu_TypeDef *menu )
{
uint8_t	i=0;
	BSP_LCD_SetTextColor(MENU_DELETE_COLOR);
	for(i=0;i<=SystemVar.current_menu[0].items;i++)
	{
		BSP_LCD_DisplayStringAt(SystemVar.current_menu[i].linex,SystemVar.current_menu[i].liney, SystemVar.current_menu[i].text, LEFT_MODE);
	}
	BSP_LCD_SetTextColor(MENU_ACTIVE_COLOR);
	for(i=0;i<=menu[0].items;i++)
	{
		BSP_LCD_DisplayStringAt(menu[i].linex,menu[i].liney, menu[i].text, LEFT_MODE);
		BSP_LCD_SetTextColor(MENU_INACTIVE_COLOR);
	}
	SystemVar.next_menu_item = 0;
	SystemVar.selected_menu_item = 0;
	SystemVar.current_menu = menu;
}

void MenuDisplayInit(void)
{
uint8_t	i=0;
	for(i=0;i< MENU_MAXLINENUM;i++)
	{
		if(MenuTop[i].liney != 0 )
		{
			SystemVar.current_menu = (Menu_TypeDef * )&MenuTop;
			BSP_LCD_SetTextColor(MenuTop[i].color);
			BSP_LCD_DisplayStringAt(MenuTop[i].linex,MenuTop[i].liney, MenuTop[i].text, LEFT_MODE);
			MenuTop[0].items = i;
		}
		else
			break;
	}
	for(i=0;i< MENU_MAXLINENUM;i++)
	{
		if(MenuSamples[i].liney != 0 )
			MenuSamples[0].items = i;
		else
			break;
	}
	for(i=0;i< MENU_MAXLINENUM;i++)
	{
		if(MenuSequence[i].liney != 0 )
			MenuSequence[0].items = i;
		else
			break;
	}
	for(i=0;i< MENU_MAXLINENUM;i++)
	{
		if(MenuDelay[i].liney != 0 )
			MenuDelay[0].items = i;
		else
			break;
	}
	for(i=0;i< MENU_MAXLINENUM;i++)
	{
		if(MenuSettings[i].liney != 0 )
			MenuSettings[0].items = i;
		else
			break;
	}
}

void MenuEncoderNavigate(void)
{
	if ( SystemVar.encval > SystemVar.last_encval )
	{
		if ( SystemVar.next_menu_item == 0 )
			SystemVar.next_menu_item = SystemVar.current_menu[0].items;
		else
			SystemVar.next_menu_item--;
	}
	else
	{
		SystemVar.next_menu_item++;
		if ( SystemVar.next_menu_item > SystemVar.current_menu[0].items )
			SystemVar.next_menu_item = 0;
	}
	MenuHilightItem(SystemVar.current_menu);
	SystemVar.encoder_flag = 0;
}

void MeuEncoderChangeMenu(void)
{
	switch(SystemVar.menu_state)
	{
	case	MENU_TOP	:
		if ( SystemVar.next_menu_item == 0 )
		{
			MenuDisplayMenu(MenuSamples);
			SystemVar.menu_state = MENU_SAMPLES;
		}
		if ( SystemVar.next_menu_item == 1 )
		{
			MenuDisplayMenu(MenuSequence);
			SystemVar.menu_state = MENU_SEQUENCE;
		}
		if ( SystemVar.next_menu_item == 2 )
		{
			MenuDisplayMenu(MenuDelay);
			SystemVar.menu_state = MENU_DELAY;
		}
		if ( SystemVar.next_menu_item == 3 )
		{
			MenuDisplayMenu(MenuSettings);
			SystemVar.menu_state = MENU_SETTINGS;
		}
		break;
	case	MENU_SAMPLES	:
		if ( SystemVar.next_menu_item == 0 )
			ReadDescriptorFileFromUSB(MENU_LINE_0_Y+MENU_FONT_HEIGHT+MenuSamples[0].items*MENU_FONT_HEIGHT);
		if ( SystemVar.next_menu_item == 2 )
			QSPI_ParseWavUSB_AndWrite();
		if ( SystemVar.next_menu_item == MenuSamples[0].items)
		{
			SystemVar.menu_state = MENU_TOP;
			ClearDescriptorFileArea(MENU_LINE_0_Y+MenuSamples[0].items*MENU_FONT_HEIGHT);
			MenuDisplayMenu(MenuTop);
		}
		break;
	case	MENU_SEQUENCE	:
		if ( SystemVar.next_menu_item == 2 )
			QSPI_ParseSeqUSB_AndWrite();
		if ( SystemVar.next_menu_item == MenuSequence[0].items)
		{
			SystemVar.menu_state = MENU_TOP;
			MenuDisplayMenu(MenuTop);
		}
		break;
	case	MENU_DELAY	:
		if ( SystemVar.next_menu_item == 0)
		{
			SystemVar.system |= SYSTEM_DELAYVAL_INCDEC;
			SystemVar.system &= ~SYSTEM_MENU_INCDEC;
			Delay_Draw(1);
			return;
		}
		if ( SystemVar.next_menu_item == 1)
		{
			if (( SystemVar.delay_type & DELAY_TYPE_FLANGER) == DELAY_TYPE_FLANGER)
			{
				SystemVar.delay_type |= DELAY_TYPE_ECHO;
				SystemVar.delay_type &= ~DELAY_TYPE_FLANGER;
			}
			else if (( SystemVar.delay_type & DELAY_TYPE_ECHO) == DELAY_TYPE_ECHO)
			{
				SystemVar.delay_type &= ~DELAY_TYPE_FLANGER;
				SystemVar.delay_type &= ~DELAY_TYPE_ECHO;
			}
			else
			{
				SystemVar.delay_type |= DELAY_TYPE_FLANGER;
			}
			DelayTypeDisplay();
			return;
		}

		if ( SystemVar.next_menu_item == MenuDelay[0].items)
		{
			SystemVar.menu_state = MENU_TOP;
			MenuDisplayMenu(MenuTop);
		}
		break;
	case	MENU_SETTINGS	:
		if ( SystemVar.next_menu_item == MenuSettings[0].items)
		{
			SystemVar.menu_state = MENU_TOP;
			MenuDisplayMenu(MenuTop);
			return;
		}
		if ( SystemVar.next_menu_item == 0)
		{
			SystemVar.system |= SYSTEM_INTEXT_SEQUENCER;
			return;
		}
		if ( SystemVar.next_menu_item == 1)
		{
			SystemVar.system &= ~SYSTEM_INTEXT_SEQUENCER;
			return;
		}
		if ( SystemVar.next_menu_item == 2)
		{
			SystemVar.system |= SYSTEM_BPM_INCDEC;
			SystemVar.system &= ~SYSTEM_MENU_INCDEC;
			BPM_Draw(1);
			return;
		}
		break;
	}
}



