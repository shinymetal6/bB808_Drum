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
	if ( BSP_QSPI_Erase_4kSector(QSPI_SEQUENCER_ADDRESS) == 0 )
		return BSP_QSPI_Write((uint8_t *)&Sequencer_Descriptor, QSPI_SEQUENCER_ADDRESS, sizeof(Sequencer_Descriptor));
	return 1;
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
uint32_t	params=0,j,val,line_idx=0,end=0;
uint8_t 	len;
uint8_t 	delay_type[16];
int		 	seq[16];

	if(f_open(&ConfFile, "bb_seq.txt", FA_OPEN_EXISTING | FA_READ) == FR_OK)
	{
		BSP_LCD_DisplayStringAt(0, STORE_MSG_MESSAGE,(uint8_t *) "Reading sequence", CENTER_MODE);
		while(end == 0)
		{
			f_gets((char * )line,256,&ConfFile);
			if ( (len = strlen((char * )line)) != 0 )
			{
				if ( params == 0 )
				{
					if ( sscanf((char * )line,"BEAT %d",(int *)&seq[0]) == 1 )
					{
						if ( seq[0] <= MAX_BEAT)
							Sequencer_Descriptor.sequencer_beat = seq[0];
					}
					else if ( sscanf((char * )line,"DELAY %s %d %d",delay_type,(int *)&seq[0],(int *)&seq[1]) == 3 )
					{
						if ( strcmp((char * )delay_type,"ECHO") == 0 )
							Sequencer_Descriptor.delay_type = DELAY_TYPE_ECHO;
						else if ( strcmp((char * )delay_type,"FLANGER") == 0 )
							Sequencer_Descriptor.delay_type = DELAY_TYPE_FLANGER;
						else
							Sequencer_Descriptor.delay_type = DELAY_TYPE_NONE;
						if ( seq[0] <= MAX_DELAY)
							Sequencer_Descriptor.delay = seq[0];
						else
							Sequencer_Descriptor.delay = 0;
						if (( seq[1] <= MAX_DELAYW) && ( seq[1] >= MIN_DELAYW))
							Sequencer_Descriptor.delay_weight = seq[1];
						else
							Sequencer_Descriptor.delay_weight = (MAX_DELAYW+MIN_DELAYW)/2;
					}
					else if ( sscanf((char * )line,"BB808_SEQUENCE %d",(int *)&seq[0]) == 1 )
					{
						Sequencer_Descriptor.sequencer_length = seq[0];
						params = 1;
					}
				}
				else
				{
					if ( sscanf((char * )line,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",&seq[0],&seq[1],&seq[2],&seq[3],&seq[4],&seq[5],&seq[6],&seq[7],&seq[8],&seq[9],&seq[10],&seq[11],&seq[12],&seq[13],&seq[14],&seq[15]) == 16 )
					{
						val = 0;
						for(j=0;j<16;j++)
						{
							if ( seq[j] != 0 )
								val |= (1 << j);
						}
						Sequencer_Descriptor.sequencer_steps[line_idx] = val;
						line_idx++;
						if ( line_idx >= Sequencer_Descriptor.sequencer_length )
							end = 1;
					}
				}
			}
		}
		Sequencer_Descriptor.flag = 0xdead;
		sprintf((char *)Sequencer_Descriptor.header,"Seq_8xx");
		if ( QSPI_WriteSeqFromUSB() != 0 )
		{
			BSP_LCD_DisplayStringAt(0, STORE_MSG_ERRORLINE,(uint8_t *)"Error storing bb_seq.txt", LEFT_MODE);
		}
		else
			BSP_LCD_DisplayStringAt(0, STORE_MSG_LINE,(uint8_t *) "                           ", LEFT_MODE);
		f_close(&ConfFile);
	}
	BSP_LCD_DisplayStringAt(0, STORE_MSG_MESSAGE,(uint8_t *)"                           ", CENTER_MODE);
	BSP_LCD_DisplayStringAt(0, STORE_MSG_LINE,(uint8_t *) "                           ", LEFT_MODE);
	SystemVar.beat = Sequencer_Descriptor.sequencer_beat;
	SystemVar.delay_type = Sequencer_Descriptor.delay_type;
	SystemVar.delay = Sequencer_Descriptor.delay;
	SystemVar.delay_weight = (float )Sequencer_Descriptor.delay_weight / 100.0F;;
	SystemVar.sequencer_length = Sequencer_Descriptor.sequencer_length;

	BPM_Draw(0);
	DelayTypeDisplay();
	Delay_Draw(0);
}
