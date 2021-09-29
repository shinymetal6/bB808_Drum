/*
 * qspi_sample_store.c
 *
 *  Created on: Sep 17, 2021
 *      Author: fil
 */
#include "main.h"
#include "qspi_sample_store.h"
#include "qspi_sample_manager.h"
#include "sequencer.h"

#include "fatfs.h"

uint8_t	line[256];
FIL								WavFile,ConfFile;
QSPISample_DescriptorTypeDef	QSPISample_Descriptor;
SampleWAV_FormatTypeDef 		wav_info;
uint8_t							qspi_buf[SECTOR4K];
uint16_t						qspi_sec_buf[SECTOR4K/2];


extern	QSPISample_HeaderTypeDef QSPISample_Header[NUM_INSTRUMENT];

uint32_t QSPI_WriteSeqFromUSB(void)
{
uint32_t	i;
	qspi_sec_buf[0] = 's'<< 8 | 'e';
	qspi_sec_buf[1] = 'q'<<8 | '8';
	for(i=0;i<SystemVar.sequencer_length;i++)
		qspi_sec_buf[i+2] = sequencer_steps[i];
	qspi_sec_buf[i+2] = 'e' << 8 | 'n';
	qspi_sec_buf[i+3] = 'd' << 8 | '8';
	if ( BSP_QSPI_Erase_4kSector(QSPI_SEQUENCER_ADDRESS) != 0 )
		return 1;
	if ( BSP_QSPI_Write((uint8_t *)qspi_sec_buf, QSPI_SEQUENCER_ADDRESS, (SystemVar.sequencer_length*2)+8) != 0 )
		return 1;
	return 0;
}


uint32_t QSPI_WriteWavFromUSB(uint8_t instrument_number , uint8_t *filename , uint8_t midi_key)
{
uint32_t 	bytesread , sector_number=0;
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
			sprintf((char *)lcdstring,"%d 0x%02x %s",
					instrument_number,
							(int )  QSPISample_Header[instrument_number].midi_key,
							(char *)QSPISample_Header[instrument_number].wav_name);
			BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
			BSP_LCD_DisplayStringAt(0, STORE_MSG_LINE, lcdstring, LEFT_MODE);

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
					BSP_QSPI_Erase_4kSector(instrument_address);
					BSP_QSPI_Write(qspi_buf, instrument_address, bytesread);
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

void QSPI_ParseWavUSB_AndWrite(void)
{
uint32_t	i;
int			instrument_number,midi_key;
uint8_t 	filename[32],len , lcdline[64];

	if(f_open(&ConfFile, "bb_sam.txt", FA_OPEN_EXISTING | FA_READ) == FR_OK)
	{
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		BSP_LCD_DisplayStringAt(0, STORE_MSG_MESSAGE, (uint8_t *)"Writing FLASH", CENTER_MODE);
		for(i=0;i<NUM_INSTRUMENT;i++)
		{
			f_gets((char * )line,256,&ConfFile);
			if ( (len = strlen((char * )line)) != 0 )
			{
				if ( sscanf((char * )line,"%d %s %d",&instrument_number,filename,&midi_key) == 3 )
				{
					if ( QSPI_WriteWavFromUSB(instrument_number,filename,midi_key) != 0 )
					{
						sprintf((char *)lcdline,"Error reading %s",filename);
						BSP_LCD_DisplayStringAt(0, STORE_MSG_ERRORLINE,lcdline, LEFT_MODE);
					}
					else
						BSP_LCD_DisplayStringAt(0, STORE_MSG_LINE,(uint8_t *) "                           ", LEFT_MODE);
				}
			}
		}
		f_close(&ConfFile);
	}
	BSP_LCD_DisplayStringAt(0, STORE_MSG_MESSAGE,(uint8_t *) "             ", CENTER_MODE);
	BSP_LCD_DisplayStringAt(0, STORE_MSG_LINE,(uint8_t *) "                           ", LEFT_MODE);
}

void QSPI_ParseSeqUSB_AndWrite(void)
{
uint32_t	i,j,seq_header = 0,val,line_idx=0,sequencer_length;
uint8_t 	len;
int		 	seq[14];

	if(f_open(&ConfFile, "bb_seq.txt", FA_OPEN_EXISTING | FA_READ) == FR_OK)
	{
		BSP_LCD_DisplayStringAt(0, STORE_MSG_MESSAGE,(uint8_t *) "Reading sequence", CENTER_MODE);
		SystemVar.sequencer_length = SEQUENCER_MAX_SIZE;
		for(i=0;i<SEQUENCER_MAX_SIZE;i++)
		{
			if ( i  > SystemVar.sequencer_length )
				break;
			f_gets((char * )line,256,&ConfFile);
			if ( (len = strlen((char * )line)) != 0 )
			{
				if ( seq_header == 0 )
				{
					if ( sscanf((char * )line,"LOOPLEN %d",(int *)&sequencer_length) == 1 )
					{
						seq_header = line_idx = 1;
						sequencer_steps[0] = sequencer_length;
					}
				}
				else
				{
					if ( sscanf((char * )line,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d",&seq[0],&seq[1],&seq[2],&seq[3],&seq[4],&seq[5],&seq[6],&seq[7],&seq[8],&seq[9],&seq[10],&seq[11],&seq[12],&seq[13]) == 14 )
					{
						val = 0;
						for(j=0;j<14;j++)
						{
							if ( seq[j] != 0 )
								val |= (1 << j);
						}
						sequencer_steps[line_idx] = val;
						line_idx++;
					}
				}
			}
		}
		SystemVar.sequencer_length = sequencer_length;
		if ( QSPI_WriteSeqFromUSB() != 0 )
		{
			BSP_LCD_DisplayStringAt(0, STORE_MSG_ERRORLINE,(uint8_t *)"Error reading bb_seq.txt", LEFT_MODE);
		}
		else
			BSP_LCD_DisplayStringAt(0, STORE_MSG_LINE,(uint8_t *) "                           ", LEFT_MODE);
		f_close(&ConfFile);
	}
	BSP_LCD_DisplayStringAt(0, STORE_MSG_MESSAGE,(uint8_t *)"                           ", CENTER_MODE);
	BSP_LCD_DisplayStringAt(0, STORE_MSG_LINE,(uint8_t *) "                           ", LEFT_MODE);
}
