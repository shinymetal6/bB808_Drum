#include "main.h"
#include "wavplayer.h"
#include "fatfs.h"

#ifdef WAVPLAYER

static AUDIO_OUT_BufferTypeDef  BufferCtl;
static uint32_t uwVolume = 70;
WAVE_FormatTypeDef WaveFormat;
FIL WavFile;
__attribute__ ((aligned (16)))  uint16_t file_buf[NUM_INSTRUMENT][AUDIO_OUT_BUFFER_SIZE];
__attribute__ ((aligned (16)))  uint16_t out_buf[AUDIO_OUT_BUFFER_SIZE];


static AUDIO_ErrorTypeDef GetFileInfo(uint8_t * filename, WAVE_FormatTypeDef *info)
{
uint32_t bytesread;
uint32_t duration;
uint8_t str[FILEMGR_FILE_NAME_SIZE + 20];

	if(f_open(&WavFile, (char *)filename, FA_OPEN_EXISTING | FA_READ) == FR_OK)
	{
		/* Fill the buffer to Send */
		if(f_read(&WavFile, info, sizeof(WaveFormat), (void *)&bytesread) == FR_OK)
		{
			BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
			sprintf((char *)str,  "Sample rate : %d Hz", (int)(info->SampleRate));
			BSP_LCD_ClearStringLine(6);
			BSP_LCD_DisplayStringAtLine(6, str);

			sprintf((char *)str,  "Channels number : %d", info->NbrChannels);
			BSP_LCD_ClearStringLine(7);
			BSP_LCD_DisplayStringAtLine(7, str);

			duration = info->FileSize / info->ByteRate;
			sprintf((char *)str, "File Size : %d KB [%02d:%02d]", (int)(info->FileSize/1024), (int)(duration/60), (int)(duration%60));
			BSP_LCD_ClearStringLine(8);
			BSP_LCD_DisplayStringAtLine(8, str);
			BSP_LCD_DisplayStringAt(31, LINE(8), (uint8_t *)"[00:00]", LEFT_MODE);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			sprintf((char *)str,  "Volume : %lu", uwVolume);
			BSP_LCD_ClearStringLine(9);
			BSP_LCD_DisplayStringAtLine(9, str);
			return AUDIO_ERROR_NONE;
		}
		f_close(&WavFile);
	}
	return AUDIO_ERROR_IO;
}


void AUDIO_PLAYER_Start(uint8_t * filename)
{
uint8_t		instrument_number = 0;

	BSP_QSPI_Init();
	GetFileInfo(filename,&WaveFormat);
	if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_BOTH, uwVolume, WaveFormat.SampleRate) != 0)
		return;
	BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
	BufferCtl.fptr[instrument_number] = 0;
	BufferCtl.sample_len[instrument_number] = WaveFormat.FileSize;
	BSP_AUDIO_OUT_Play(out_buf, AUDIO_OUT_BUFFER_SIZE);
	return;
}

//#define	FROM_QSPI	1
uint32_t get_data(FIL *WavFile , uint8_t *data, uint32_t size , uint32_t *data_received, uint32_t fptr)
{
#ifdef FROM_QSPI
#define	FILE_POINTER	(1024*1024*6)
	BSP_QSPI_Read((uint8_t *)data, FILE_POINTER + fptr, size);
	*data_received = size;
#else
uint32_t bytesread;
	f_read(WavFile,	(uint8_t *)data ,	size,	(void *)&bytesread);
	*data_received = bytesread;
#endif
	return 0;

}

void Sample_Process(void)
{
uint32_t bytesread,i;
uint8_t		instrument_number = 0;

	HAL_GPIO_WritePin(ARD_D8_GPIO_GPIO_Port, ARD_D8_GPIO_Pin, GPIO_PIN_SET);
#ifdef FROM_QSPI
	__disable_irq();
#endif
	//if(BufferCtl.fptr[instrument_number] >= WaveFormat.FileSize)
	if(BufferCtl.fptr[instrument_number] >= BufferCtl.sample_len[instrument_number])
	{
		BufferCtl.fptr[instrument_number] = 0;
		BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
		HAL_GPIO_WritePin(ARD_D8_GPIO_GPIO_Port, ARD_D8_GPIO_Pin, GPIO_PIN_RESET);
		return;
	}

	if (( BufferCtl.flag &  WAVPLAY_STATE_FLAG_HALF ) == WAVPLAY_STATE_FLAG_HALF)
	{
		//if(f_read(&WavFile,	(uint8_t *)file_buf[instrument_number] ,	AUDIO_OUT_BUFFER_SIZE,	(void *)&bytesread) != FR_OK)
		get_data(&WavFile,	(uint8_t *)file_buf[instrument_number] ,	AUDIO_OUT_BUFFER_SIZE,	(void *)&bytesread , BufferCtl.fptr[instrument_number]);
		BufferCtl.flag &= ~WAVPLAY_STATE_FLAG_HALF;
		BufferCtl.fptr[instrument_number] += bytesread;
		for(i=0;i<AUDIO_OUT_HALFBUFFER_SIZE;i++)
			out_buf[i] = file_buf[instrument_number][i];
	}

	if (( BufferCtl.flag &  WAVPLAY_STATE_FLAG_FULL ) == WAVPLAY_STATE_FLAG_FULL)
	{
		//if(f_read(&WavFile,	(uint8_t *)&file_buf[instrument_number] [0],AUDIO_OUT_BUFFER_SIZE,(void *)&bytesread) != FR_OK)
		get_data(&WavFile,	(uint8_t *)file_buf[instrument_number],AUDIO_OUT_BUFFER_SIZE,(void *)&bytesread, BufferCtl.fptr[instrument_number]);
		BufferCtl.flag &= ~WAVPLAY_STATE_FLAG_FULL;
		BufferCtl.fptr[instrument_number] += bytesread;
		for(i=AUDIO_OUT_HALFBUFFER_SIZE;i<AUDIO_OUT_BUFFER_SIZE;i++)
			out_buf[i] = file_buf[instrument_number] [i-AUDIO_OUT_HALFBUFFER_SIZE];
	}
	__enable_irq();
	HAL_GPIO_WritePin(ARD_D8_GPIO_GPIO_Port, ARD_D8_GPIO_Pin, GPIO_PIN_RESET);

	return ;
}


void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
	/* allows AUDIO_Process() to refill 2nd part of the buffer  */
	BufferCtl.flag |= WAVPLAY_STATE_FLAG_FULL;
	Sample_Process();
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
	/* allows AUDIO_Process() to refill 1st part of the buffer  */
	BufferCtl.flag |= WAVPLAY_STATE_FLAG_HALF;
	Sample_Process();
}

void BSP_AUDIO_OUT_ClockConfig(SAI_HandleTypeDef *hsai, uint32_t AudioFreq, void *Params)
{
RCC_PeriphCLKInitTypeDef rcc_ex_clk_init_struct;

	HAL_RCCEx_GetPeriphCLKConfig(&rcc_ex_clk_init_struct);
	switch (AudioFreq )
	{
	case	AUDIO_FREQUENCY_22K	:
		rcc_ex_clk_init_struct.PLLI2S.PLLI2SN = 316;
		rcc_ex_clk_init_struct.PLLI2S.PLLI2SQ = 14;
		rcc_ex_clk_init_struct.PLLI2SDivQ = 8;
		break;
	case	AUDIO_FREQUENCY_44K	:
		rcc_ex_clk_init_struct.PLLI2S.PLLI2SN = 316;
		rcc_ex_clk_init_struct.PLLI2S.PLLI2SQ = 14;
		rcc_ex_clk_init_struct.PLLI2SDivQ = 4;
		break;
	case	AUDIO_FREQUENCY_48K	:
		rcc_ex_clk_init_struct.PLLI2S.PLLI2SN = 344;
		rcc_ex_clk_init_struct.PLLI2S.PLLI2SQ = 14;
		rcc_ex_clk_init_struct.PLLI2SDivQ = 4;
		break;
	default	:
		rcc_ex_clk_init_struct.PLLI2S.PLLI2SN = 344;
		rcc_ex_clk_init_struct.PLLI2S.PLLI2SQ = 14;
		rcc_ex_clk_init_struct.PLLI2SDivQ = 4;
		break;
	}

	rcc_ex_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
	rcc_ex_clk_init_struct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
	HAL_RCCEx_PeriphCLKConfig(&rcc_ex_clk_init_struct);
}

#endif
