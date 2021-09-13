#include "main.h"
#include "wavplayer.h"
#include "fatfs.h"

#ifdef WAVPLAYER

static AUDIO_OUT_BufferTypeDef  BufferCtl;
static uint32_t uwVolume = 70;
WAVE_FormatTypeDef WaveFormat;
FIL WavFile;
__attribute__ ((aligned (16)))  uint16_t file_buf[AUDIO_OUT_BUFFER_SIZE];


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

uint8_t PlayerInit(uint32_t AudioFreq)
{
	/* Initialize the Audio codec and all related peripherals (I2S, I2C, IOExpander, IOs...) */
	if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_BOTH, uwVolume, AudioFreq) != 0)
		return 1;
	BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
	return 0;
}

AUDIO_ErrorTypeDef AUDIO_PLAYER_Start(uint8_t * filename)
{
uint32_t bytesread;

	GetFileInfo(filename,&WaveFormat);
	PlayerInit(WaveFormat.SampleRate);

	f_lseek(&WavFile, 0);

	/* Fill whole buffer at first time */
	if(f_read(&WavFile,(uint8_t *)file_buf,AUDIO_OUT_BUFFER_SIZE*2,(void *)&bytesread) == FR_OK)
	{
		BSP_LCD_DisplayStringAt(18, LINE(10), (uint8_t *)"  [PLAY ]", LEFT_MODE);
		if(bytesread != 0)
		{
			BSP_AUDIO_OUT_Play(file_buf, AUDIO_OUT_BUFFER_SIZE);
			BufferCtl.fptr = bytesread;
			return AUDIO_ERROR_NONE;
		}
	}
	return AUDIO_ERROR_IO;
}

AUDIO_ErrorTypeDef AUDIO_PLAYER_Process(void)
{
AUDIO_ErrorTypeDef audio_error = AUDIO_ERROR_NONE;
uint32_t bytesread;

	HAL_GPIO_WritePin(ARD_D8_GPIO_GPIO_Port, ARD_D8_GPIO_Pin, GPIO_PIN_SET);

	if(BufferCtl.fptr >= WaveFormat.FileSize)
	{
		BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
		return audio_error;
	}
	if (( BufferCtl.flag &  WAVPLAY_STATE_FLAG_HALF ) == WAVPLAY_STATE_FLAG_HALF)
	{
		if(f_read(&WavFile,	(uint8_t *)file_buf,	AUDIO_OUT_BUFFER_SIZE,	(void *)&bytesread) != FR_OK)
		{
			BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
			return AUDIO_ERROR_IO;
		}
		BufferCtl.flag &= ~WAVPLAY_STATE_FLAG_HALF;
		BufferCtl.fptr += bytesread;
	}

	if (( BufferCtl.flag &  WAVPLAY_STATE_FLAG_FULL ) == WAVPLAY_STATE_FLAG_FULL)
	{
		if(f_read(&WavFile,	(uint8_t *)&file_buf[AUDIO_OUT_HALFBUFFER_SIZE],AUDIO_OUT_BUFFER_SIZE,(void *)&bytesread) != FR_OK)
		{
			BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
			return AUDIO_ERROR_IO;
		}
		BufferCtl.flag &= ~WAVPLAY_STATE_FLAG_FULL;
		BufferCtl.fptr += bytesread;
	}
	HAL_GPIO_WritePin(ARD_D8_GPIO_GPIO_Port, ARD_D8_GPIO_Pin, GPIO_PIN_RESET);
	return audio_error;
}

AUDIO_ErrorTypeDef AUDIO_PLAYER_Stop(void)
{
	BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
	f_close(&WavFile);
	return AUDIO_ERROR_NONE;
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
	/* allows AUDIO_Process() to refill 2nd part of the buffer  */
	BufferCtl.flag |= WAVPLAY_STATE_FLAG_FULL;
	AUDIO_PLAYER_Process();
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
	/* allows AUDIO_Process() to refill 1st part of the buffer  */
	BufferCtl.flag |= WAVPLAY_STATE_FLAG_HALF;
	AUDIO_PLAYER_Process();
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

#endif
