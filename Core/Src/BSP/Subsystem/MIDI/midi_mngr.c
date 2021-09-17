/*
 * midi_mngr.c
 *
 *  Created on: Aug 12, 2021
 *      Author: fil
 */

#include "main.h"
#include "usb_host.h"
#include "usbh_def.h"
#include "usbh_MIDI.h"

uint8_t	midi_buffer[64],midi_rxbuffer[64];

void MIDI_Application(uint8_t from,USBH_HandleTypeDef *phost, uint8_t id)
{
	USBH_MIDI_Receive(phost, midi_rxbuffer, 64);
}

void USB_CallFromFS(USBH_HandleTypeDef *phost, uint8_t id)
{
	if ( strncmp((char *)phost->pClass[0]->Name,"MIDI",4) == 0 )
		MIDI_Application(0,phost,id);
}

__weak void USBH_MIDI_ReceiveCallback(USBH_HandleTypeDef *phost)
{
uint8_t	i,k=0,j;
uint8_t	lcd_string[64];

	for(i=0;i<64;i++)
	{

		if (( midi_rxbuffer[i] == 0 ) && (midi_rxbuffer[i+1] == 0) && ((i & 0x03 ) == 0 ))
			i = 64;
		else if ((( midi_rxbuffer[i] & 0xf0) != 0 ) && ((i & 0x03 ) == 0 ))
			i +=3;
		else
		{
			if ((midi_rxbuffer[i] == 0x0b ) || (midi_rxbuffer[i] == 0x08 ) || (midi_rxbuffer[i] == 0x09 ))
			{
				for ( j=0;j<4 ; j++,k++,i++)
					midi_buffer[k] = midi_rxbuffer[i];
				i -= 1;
			}
		}
	}
	sprintf((char *)lcd_string, "USBH_MIDI_ReceiveCallback");
	BSP_LCD_DisplayStringAt(0, 90, (uint8_t *)lcd_string, CENTER_MODE);
	sprintf((char *)lcd_string, "0x%02x 0x%02x 0x%02x 0x%02x", midi_buffer[0],midi_buffer[1],midi_buffer[2],midi_buffer[3]);
	BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)lcd_string, CENTER_MODE);
	USBH_MIDI_Receive(phost, midi_rxbuffer, 64);
}

