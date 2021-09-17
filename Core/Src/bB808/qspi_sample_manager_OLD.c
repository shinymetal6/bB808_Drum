/*
 * qspi_sample_manager.c
 *
 *  Created on: Sep 14, 2021
 *      Author: fil
 */

#include "main.h"
#include "qspi_sample_manager.h"
#include "fatfs.h"

#ifdef QSPISAMPLEPLAYER

__attribute__ ((aligned (16)))  uint16_t file_buf[NUM_INSTRUMENT][SAMPLE_OUT_HALFBUFFER_SIZE];
__attribute__ ((aligned (16)))  uint16_t out_buf[SAMPLE_OUT_BUFFER_SIZE];
__attribute__ ((aligned (16)))  uint16_t zero_buf[SAMPLE_OUT_HALFBUFFER_SIZE];

QSPISample_DescriptorTypeDef	QSPISample_Descriptor;

FIL								WavFile,ConfFile;
uint8_t							qspi_buf[SECTOR64K];
QSPISample_HeaderTypeDef		QSPISample_Header[NUM_INSTRUMENT]; //,ReadSample_Header[NUM_INSTRUMENT];
SampleWAV_FormatTypeDef 		wav_info;

void QSPI_EraseInstrument(uint8_t instrument)
{
uint32_t	instrumet_address , i;
uint8_t	qspi_buf[32];

	instrumet_address = instrument * INSTRUMENT_SIZE;
	for(i=0;i<NUMSECTOR_PER_INSTRUMENT;i++)
		BSP_QSPI_Erase_64kBlock(instrumet_address + i*SECTOR64K);
	sprintf((char *)qspi_buf,"Empty");
	qspi_buf[31] = 0;
	BSP_QSPI_Write(qspi_buf, i*INSTRUMENT_SIZE, 32);
}

void QSPI_WriteInstrument64kBlock(uint8_t instrument_number , uint32_t block_number , uint8_t *data)
{
uint32_t	instrument_address;

	instrument_address = (instrument_number * INSTRUMENT_SIZE) + (block_number * SECTOR64K);
	BSP_QSPI_Erase_64kBlock(instrument_address);
	BSP_QSPI_Write(data, instrument_address, SECTOR64K);
}

void QSPI_ReadInstrumentBlock(uint8_t instrument_number , uint32_t block_number , uint32_t startaddr)
{
uint32_t	instrument_address;

	instrument_address = (instrument_number * INSTRUMENT_SIZE) + (block_number * SECTOR64K);
	BSP_QSPI_Read((uint8_t *)&file_buf[instrument_number][startaddr], instrument_address, SAMPLE_OUT_BUFFER_SIZE);
}

uint32_t QSPI_WriteWavFromUSB(uint8_t instrument_number , uint8_t *filename , uint8_t midi_key)
{
uint32_t 	bytesread , sector_number=0;
	if(f_open(&WavFile, (char *)filename, FA_OPEN_EXISTING | FA_READ) == FR_OK)
	{
		if(f_read(&WavFile, &wav_info, sizeof(wav_info), (void *)&bytesread) == FR_OK)
		{
			sprintf((char *)QSPISample_Header[instrument_number].wav_name,(char *)filename);
			QSPISample_Header[instrument_number].midi_key = midi_key;
			QSPISample_Header[instrument_number].wav_len = wav_info.FileSize;
			QSPISample_Header[instrument_number].sample_rate = wav_info.SampleRate;
			for(sector_number=0;sector_number < NUMSECTOR_PER_INSTRUMENT;sector_number++)
			{

				if ( sector_number == 0 )
				{
					memcpy((uint8_t *)qspi_buf,(uint8_t *)&QSPISample_Header[instrument_number],HEADER_SIZE);
					if(f_read(&WavFile,	(uint8_t *)&qspi_buf[HEADER_SIZE],SECTOR64K-HEADER_SIZE,(void *)&bytesread) != FR_OK)
						return 1;
				}
				else
				{
					if(f_read(&WavFile,	(uint8_t *)qspi_buf,SECTOR64K,	(void *)&bytesread) != FR_OK)
						return 1;
				}
				QSPI_WriteInstrument64kBlock(instrument_number,sector_number,qspi_buf);
				sector_number++;
			}
			f_close(&WavFile);
			return 0;
		}
	}
	return 1;
}

uint8_t	line[256];

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

#define	NO_WRITE	1

void QSPI_ReadDescriptorFileFromUSB(void)
{
uint32_t	i;
#ifndef NO_WRITE
int			instrument_number,midi_key;
uint8_t 	filename[32],len;
#endif
	BSP_QSPI_Init();
#ifndef NO_WRITE
	if(f_open(&ConfFile, "bB808.txt", FA_OPEN_EXISTING | FA_READ) == FR_OK)
	{
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
#endif
	QSPI_ReadAndDumpDescriptor();
	for(i=0;i< SAMPLE_OUT_HALFBUFFER_SIZE ; i++)
		zero_buf[i] = 0;
}

void QSPIInstrumentON(uint8_t instrument_number)
{
	QSPISample_Descriptor.qspi_ptr[instrument_number] = 0;
	QSPISample_Descriptor.sample_flag[instrument_number] |=  SAMPLE_ACTIVE_BIT;
}

void QSPIInstrumentOFF(uint8_t instrument_number)
{
	//QSPISample_Descriptor.sample_flag[instrument_number] &= ~SAMPLE_ACTIVE_BIT;
}

uint32_t QSPISamplePlayerStart(void)
{
uint32_t	i;
	if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_BOTH, 70, 44100) != 0)
		return 1;
	BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
	for(i=0;i<SAMPLE_OUT_HALFBUFFER_SIZE;i++)
		out_buf[i] = 0;
	BSP_AUDIO_OUT_Play(out_buf, SAMPLE_OUT_BUFFER_SIZE);
	for(i=0;i<NUM_INSTRUMENT;i++)
		QSPISample_Descriptor.sample_flag[i] &= ~SAMPLE_ACTIVE_BIT;
	return 0;
}

static void store_to_outbuf(uint16_t buf_index ,uint16_t limit,uint16_t *buf_ptr )
{
uint32_t	i;
uint16_t	sample;
	for(i=0;i<SAMPLE_OUT_HALFBUFFER_SIZE;i++)
	{
		/*
		if ( buf_index == 0 )
		{
			out_buf[i+limit] = *buf_ptr++;
		}
		else
		{
			sample = (out_buf[i+limit] >> 1 ) + ((*buf_ptr++) >> 1);
			out_buf[i+limit] = sample;
		}
		*/
		out_buf[i+limit] = *buf_ptr++;
	}
}

static void QSPISamplePlayerProcess(void)
{
uint8_t 	instrument_number;
uint32_t	sample_found = 0,i,offset;

	HAL_GPIO_WritePin(ARD_D8_GPIO_GPIO_Port, ARD_D8_GPIO_Pin, GPIO_PIN_SET);

	if (( QSPISample_Descriptor.flag &  QSPISAMPLE_STATE_FLAG_HALF ) == QSPISAMPLE_STATE_FLAG_HALF)
		offset = 0;
	else
		offset = SAMPLE_OUT_HALFBUFFER_SIZE;

	__disable_irq();
	for(instrument_number=0;instrument_number<NUM_INSTRUMENT/2;instrument_number++)
	{
		if (( QSPISample_Descriptor.sample_flag[instrument_number] &  SAMPLE_ACTIVE_BIT ) == SAMPLE_ACTIVE_BIT)
		{
			BSP_QSPI_Read((uint8_t *)&file_buf[instrument_number][offset*2], QSPISample_Descriptor.qspi_ptr[instrument_number], SAMPLE_OUT_BUFFER_SIZE);
			store_to_outbuf(instrument_number,offset,&file_buf[instrument_number][offset]);
			QSPISample_Descriptor.qspi_ptr[instrument_number] += SAMPLE_OUT_BUFFER_SIZE;
			if ( QSPISample_Descriptor.qspi_ptr[instrument_number] > QSPISample_Header[instrument_number].wav_len )
			{
				QSPISample_Descriptor.sample_flag[instrument_number] &= ~SAMPLE_ACTIVE_BIT;
				QSPISample_Descriptor.qspi_ptr[instrument_number] = 0;
				store_to_outbuf(instrument_number,offset,zero_buf);
			}
		}
		/*
		else
		{
			store_to_outbuf(instrument_number,offset,zero_buf);
		}
		*/
	}
	if (( QSPISample_Descriptor.flag &  QSPISAMPLE_STATE_FLAG_HALF ) == QSPISAMPLE_STATE_FLAG_HALF)
		QSPISample_Descriptor.flag &= ~QSPISAMPLE_STATE_FLAG_HALF;
	if (( QSPISample_Descriptor.flag &  QSPISAMPLE_STATE_FLAG_FULL ) == QSPISAMPLE_STATE_FLAG_FULL)
		QSPISample_Descriptor.flag &= ~QSPISAMPLE_STATE_FLAG_FULL;
	__enable_irq();
/*
	if (( QSPISample_Descriptor.flag &  QSPISAMPLE_STATE_FLAG_HALF ) == QSPISAMPLE_STATE_FLAG_HALF)
	{
		for(instrument_number=0;instrument_number<NUM_INSTRUMENT;instrument_number++)
		{
			if (( QSPISample_Descriptor.sample_flag[instrument_number] &  SAMPLE_ACTIVE_BIT ) == SAMPLE_ACTIVE_BIT)
			{
				__disable_irq();
				BSP_QSPI_Read((uint8_t *)&file_buf[instrument_number][0], QSPISample_Descriptor.qspi_ptr[instrument_number], SAMPLE_OUT_BUFFER_SIZE);
				store_to_outbuf(instrument_number,0);
				sample_found++;
				QSPISample_Descriptor.qspi_ptr[instrument_number] += SAMPLE_OUT_BUFFER_SIZE;
				if ( QSPISample_Descriptor.qspi_ptr[instrument_number] > QSPISample_Header[instrument_number].wav_len )
				{
					QSPISample_Descriptor.sample_flag[instrument_number] &= ~SAMPLE_ACTIVE_BIT;
					QSPISample_Descriptor.qspi_ptr[instrument_number] = 0;
				}
				__enable_irq();
			}
		}
		QSPISample_Descriptor.flag &= ~QSPISAMPLE_STATE_FLAG_HALF;
	}
	if (( QSPISample_Descriptor.flag &  QSPISAMPLE_STATE_FLAG_FULL ) == QSPISAMPLE_STATE_FLAG_FULL)
	{
		for(instrument_number=0;instrument_number<NUM_INSTRUMENT;instrument_number++)
		{
			if (( QSPISample_Descriptor.sample_flag[instrument_number] &  SAMPLE_ACTIVE_BIT ) == SAMPLE_ACTIVE_BIT)
			{
				__disable_irq();
				BSP_QSPI_Read((uint8_t *)&file_buf[instrument_number][SAMPLE_OUT_BUFFER_SIZE], QSPISample_Descriptor.qspi_ptr[instrument_number], SAMPLE_OUT_BUFFER_SIZE);
				store_to_outbuf(instrument_number,SAMPLE_OUT_HALFBUFFER_SIZE);
				sample_found++;
				QSPISample_Descriptor.qspi_ptr[instrument_number] += SAMPLE_OUT_BUFFER_SIZE;
				if ( QSPISample_Descriptor.qspi_ptr[instrument_number] > QSPISample_Header[instrument_number].wav_len )
				{
					QSPISample_Descriptor.sample_flag[instrument_number] &= ~SAMPLE_ACTIVE_BIT;
					QSPISample_Descriptor.qspi_ptr[instrument_number] = 0;
				}
				__enable_irq();
			}
		}
		QSPISample_Descriptor.flag &= ~QSPISAMPLE_STATE_FLAG_FULL;
	}
	*/

	HAL_GPIO_WritePin(ARD_D8_GPIO_GPIO_Port, ARD_D8_GPIO_Pin, GPIO_PIN_RESET);
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
	/* allows AUDIO_Process() to refill 2nd part of the buffer  */
	QSPISample_Descriptor.flag |= QSPISAMPLE_STATE_FLAG_FULL;
	QSPISamplePlayerProcess();
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
	/* allows AUDIO_Process() to refill 1st part of the buffer  */
	QSPISample_Descriptor.flag |= QSPISAMPLE_STATE_FLAG_HALF;
	QSPISamplePlayerProcess();
}

void BSP_AUDIO_OUT_ClockConfig(SAI_HandleTypeDef *hsai, uint32_t AudioFreq, void *Params)
{
  RCC_PeriphCLKInitTypeDef rcc_ex_clk_init_struct;

  HAL_RCCEx_GetPeriphCLKConfig(&rcc_ex_clk_init_struct);

  /* Set the PLL configuration according to the audio frequency */
  if((AudioFreq == AUDIO_FREQUENCY_11K) || (AudioFreq == AUDIO_FREQUENCY_22K) || (AudioFreq == AUDIO_FREQUENCY_44K))
  {
    /* Configure PLLSAI prescalers */
    /* PLLSAI_VCO: VCO_429M
    SAI_CLK(first level) = PLLSAI_VCO/PLLSAIQ = 429/2 = 214.5 Mhz
    SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ = 214.5/19 = 11.289 Mhz */
    rcc_ex_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
    rcc_ex_clk_init_struct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
    rcc_ex_clk_init_struct.PLLI2S.PLLI2SN = 429;
    //rcc_ex_clk_init_struct.PLLI2S.PLLI2SQ = 2;
    rcc_ex_clk_init_struct.PLLI2S.PLLI2SQ = 1;
    rcc_ex_clk_init_struct.PLLI2SDivQ = 19;

    HAL_RCCEx_PeriphCLKConfig(&rcc_ex_clk_init_struct);

  }
  else /* AUDIO_FREQUENCY_8K, AUDIO_FREQUENCY_16K, AUDIO_FREQUENCY_48K, AUDIO_FREQUENCY_96K */
  {
    /* SAI clock config
    PLLSAI_VCO: VCO_344M
    SAI_CLK(first level) = PLLSAI_VCO/PLLSAIQ = 344/7 = 49.142 Mhz
    SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ = 49.142/1 = 49.142 Mhz */
    rcc_ex_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
    rcc_ex_clk_init_struct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
    rcc_ex_clk_init_struct.PLLI2S.PLLI2SN = 344;
    //rcc_ex_clk_init_struct.PLLI2S.PLLI2SQ = 7;
    rcc_ex_clk_init_struct.PLLI2S.PLLI2SQ = 14;
    //rcc_ex_clk_init_struct.PLLI2SDivQ = 1;
    rcc_ex_clk_init_struct.PLLI2SDivQ = 4;

    HAL_RCCEx_PeriphCLKConfig(&rcc_ex_clk_init_struct);
  }
}

uint8_t	qspi_midi_buffer[64],qspi_midi_rxbuffer[64];
extern	USBH_StatusTypeDef  USBH_MIDI_Receive(USBH_HandleTypeDef *phost, uint8_t *pbuff, uint16_t length);
extern	uint32_t	mididev_flag;
extern	USBH_HandleTypeDef *midi_on_fire;

void USBH_MIDI_ReceiveCallback(USBH_HandleTypeDef *phost)
{
	midi_on_fire = phost;
	mididev_flag = 1;
}

void USBH_MIDI_Rx(USBH_HandleTypeDef *phost)
{
uint8_t	i,k=0,j;
uint8_t	lcd_string[64];
	for(i=0;i<64;i++)
	{

		if (( qspi_midi_buffer[i] == 0 ) && (qspi_midi_rxbuffer[i+1] == 0) && ((i & 0x03 ) == 0 ))
			i = 64;
		else if ((( qspi_midi_rxbuffer[i] & 0xf0) != 0 ) && ((i & 0x03 ) == 0 ))
			i +=3;
		else
		{
			if ((qspi_midi_rxbuffer[i] == 0x0b ) || (qspi_midi_rxbuffer[i] == 0x08 ) || (qspi_midi_rxbuffer[i] == 0x09 ))
			{
				for ( j=0;j<4 ; j++,k++,i++)
					qspi_midi_buffer[k] = qspi_midi_rxbuffer[i];
				i -= 1;
			}
		}
	}
	sprintf((char *)lcd_string, "0x%02x 0x%02x 0x%02x 0x%02x", qspi_midi_buffer[0],qspi_midi_buffer[1],qspi_midi_buffer[2],qspi_midi_buffer[3]);
	BSP_LCD_DisplayStringAt(0, 60, (uint8_t *)lcd_string, CENTER_MODE);
	if (qspi_midi_rxbuffer[0] == 0x09 )
		QSPIInstrumentON(qspi_midi_buffer[2]-0x24);
	if (qspi_midi_rxbuffer[0] == 0x08 )
		QSPIInstrumentOFF(qspi_midi_buffer[2]-0x24);

	USBH_MIDI_Receive(phost, qspi_midi_rxbuffer, 64);
}
#endif
