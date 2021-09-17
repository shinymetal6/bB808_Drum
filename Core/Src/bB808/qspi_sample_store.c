/*
 * qspi_sample_store.c
 *
 *  Created on: Sep 17, 2021
 *      Author: fil
 */
#include "main.h"
#include "qspi_sample_store.h"
#include "qspi_sample_manager.h"

#include "fatfs.h"

uint8_t	line[256];
FIL								WavFile,ConfFile;
QSPISample_DescriptorTypeDef	QSPISample_Descriptor;
SampleWAV_FormatTypeDef 		wav_info;
uint8_t							qspi_buf[SECTOR64K];


extern	QSPISample_HeaderTypeDef QSPISample_Header[NUM_INSTRUMENT];


void print_error(uint32_t type , uint32_t address)
{
uint32_t	lcdline=40;
uint8_t		lcdstring[32];

	BSP_LCD_SetTextColor(LCD_COLOR_RED);

	if ( type == 0 )
		sprintf((char *)lcdstring,"Error clear @ 0x%08x",(int )address);
	else
		sprintf((char *)lcdstring,"Error write @ 0x%08x",(int )address);
	BSP_LCD_DisplayStringAt(0, lcdline, lcdstring, LEFT_MODE);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
}

uint32_t QSPI_WriteWavFromUSB(uint8_t instrument_number , uint8_t *filename , uint8_t midi_key)
{
uint32_t 	bytesread , sector_number=0;
uint32_t	lcdline=40;
uint8_t		lcdstring[32];
uint32_t	instrument_address;

	if(f_open(&WavFile, (char *)filename, FA_OPEN_EXISTING | FA_READ) == FR_OK)
	{
		if(f_read(&WavFile, &wav_info, sizeof(wav_info), (void *)&bytesread) == FR_OK)
		{
			sprintf((char *)QSPISample_Header[instrument_number].wav_name,(char *)filename);
			QSPISample_Header[instrument_number].midi_key = midi_key;
			QSPISample_Header[instrument_number].wav_len = wav_info.FileSize;
			QSPISample_Header[instrument_number].sample_rate = wav_info.SampleRate;
			sprintf((char *)lcdstring,"%d %d 0x%02x %s",
					instrument_number,
							(int )  QSPISample_Header[instrument_number].sample_rate,
							(int )  QSPISample_Header[instrument_number].midi_key,
							(char *)QSPISample_Header[instrument_number].wav_name);
			BSP_LCD_DisplayStringAt(0, lcdline, lcdstring, LEFT_MODE);

			for(sector_number=0;sector_number < NUMSECTOR4K_PER_INSTRUMENT;sector_number++)
			{
				if ( sector_number == 0 )
				{
					memcpy((uint8_t *)qspi_buf,(uint8_t *)&QSPISample_Header[instrument_number],HEADER_SIZE);
					if(f_read(&WavFile,	(uint8_t *)&qspi_buf[HEADER_SIZE],SECTOR4K-HEADER_SIZE,(void *)&bytesread) != FR_OK)
						return 1;
					bytesread += HEADER_SIZE;
				}
				else
				{
					if(f_read(&WavFile,	(uint8_t *)qspi_buf,SECTOR4K,	(void *)&bytesread) != FR_OK)
						return 1;
				}
				if ( bytesread != 0 )
				{
					instrument_address = (instrument_number * INSTRUMENT_SIZE) + (sector_number * SECTOR4K);
					__disable_irq();
					if ( BSP_QSPI_Erase_4kSector(instrument_address) != 0 )
						print_error(0,instrument_address);
					if ( BSP_QSPI_Write(qspi_buf, instrument_address, bytesread) )
						print_error(1,instrument_address);
					__enable_irq();
				}
				HAL_Delay(1);
			}
			f_close(&WavFile);
			return 0;
		}
	}
	return 1;
}

void QSPI_ReadAndDumpDescriptor(void)
{
uint8_t 	i;
uint8_t		lcdstring[32];
uint32_t	instrument_address;
uint32_t	lcdline=90;

	for(i=0;i<NUM_INSTRUMENT;i++,lcdline+=10)
	{
		instrument_address = i * INSTRUMENT_SIZE;
		BSP_QSPI_Read((uint8_t *)&QSPISample_Header[i], instrument_address, HEADER_SIZE);
		sprintf((char *)lcdstring,"%d %d 0x%02x %s",
				i,
				(int )  QSPISample_Header[i].sample_rate,
				(int )  QSPISample_Header[i].midi_key,
				(char *)QSPISample_Header[i].wav_name);
		BSP_LCD_DisplayStringAt(0, lcdline, lcdstring, LEFT_MODE);
		QSPISample_Descriptor.midi_key[i] = QSPISample_Header[i].midi_key;
		QSPISample_Descriptor.sample_len[i] = QSPISample_Header[i].wav_len;
		QSPISample_Descriptor.qspi_ptr[i] = 0;
	}
}

void QSPI_ReadDescriptorFileFromUSB(void)
{
uint32_t	i;
int			instrument_number,midi_key;
uint8_t 	filename[32],len;

	BSP_QSPI_Init();
	if(f_open(&ConfFile, "bB808.txt", FA_OPEN_EXISTING | FA_READ) == FR_OK)
	{
		BSP_LCD_DisplayStringAt(0, 30, (uint8_t *)"Found bB808.txt, writing flash", LEFT_MODE);

		for(i=0;i<NUM_INSTRUMENT;i++)
		{
			f_gets((char * )line,256,&ConfFile);
			if ( (len = strlen((char * )line)) != 0 )
			{
				if ( sscanf((char * )line,"%d %s %d",&instrument_number,filename,&midi_key) == 3 )
				{
					if ( QSPI_WriteWavFromUSB(instrument_number,filename,midi_key) != 0 )
						return;
				}
			}
		}
		f_close(&ConfFile);
	}
}

