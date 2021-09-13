/*
 * sample_mngr.c
 *
 *  Created on: Sep 13, 2021
 *      Author: fil
 */

#include "main.h"
#include "fatfs.h"

#ifdef SAMPLEPLAYER
Sample_DescriptorTypeDef		Sample_Descriptor;
__attribute__ ((aligned (16)))  uint16_t file_buf[MAX_NUM_SAMPLES][SAMPLE_OUT_BUFFER_SIZE];
__attribute__ ((aligned (16)))  uint16_t out_buf[SAMPLE_OUT_BUFFER_SIZE];
SampleWAV_FormatTypeDef 		SampleWAV_Format;
FIL 							WavFile[MAX_NUM_SAMPLES];
static uint32_t 				SampleVolume = 70;

Sample_WavFilesTypeDef	Sample_WavFiles[MAX_NUM_SAMPLES] =
{
		{
				"sample.wav",
				0x25,
		},
		{
				"sample.wav",
				0x26,
		},
		{
				"sample.wav",
				0x27,
		},
		{
				"sample.wav",
				0x28,
		},
		{
				"sample.wav",
				0x29,
		},
		{
				"sample.wav",
				0x2a,
		},
		{
				"sample.wav",
				0x2b,
		},
		{
				"sample.wav",
				0x2c,
		},
};

static uint32_t SampleGetFileInfo(uint8_t * filename, SampleWAV_FormatTypeDef *info , uint8_t index)
{
uint32_t bytesread;
#ifdef	PRINT_FILE_INFO
uint32_t duration;
uint8_t str[64];
#endif


	if(f_open(&WavFile[index], (char *)filename, FA_OPEN_EXISTING | FA_READ) == FR_OK)
	{
		if(f_read(&WavFile[index], info, sizeof(SampleWAV_Format), (void *)&bytesread) == FR_OK)
		{
#ifdef	PRINT_FILE_INFO
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
			sprintf((char *)str,  "Volume : %lu", SampleVolume);
			BSP_LCD_ClearStringLine(9);
			BSP_LCD_DisplayStringAtLine(9, str);
#endif
			return 0;
		}
		f_close(&WavFile[index]);
	}
	return 1;
}

uint32_t SamplePlayerInit(uint32_t AudioFreq)
{
	/* Initialize the Audio codec and all related peripherals (I2S, I2C, IOExpander, IOs...) */
	if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_BOTH, SampleVolume, AudioFreq) != 0)
		return 1;
	BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
	return 0;
}


uint32_t SamplePlayerStart(uint8_t * filename)
{
uint32_t	i;
SampleWAV_FormatTypeDef info;

	for(i=0;i<MAX_NUM_SAMPLES;i++)
	{
		if ( Sample_WavFiles[i].midi_key == 0 )
			i = MAX_NUM_SAMPLES;
		else
		{
			if ( SampleGetFileInfo(filename,&info,i) == 0 )
			{
				Sample_Descriptor.sample_rate[i] = info.SampleRate;
				Sample_Descriptor.midi_key[i] = Sample_WavFiles[i].midi_key;
			}
		}
	}
	SamplePlayerInit(SampleWAV_Format.SampleRate);

	for(i=0;i<SAMPLE_OUT_HALFBUFFER_SIZE;i++)
		out_buf[i] = 0;
	BSP_AUDIO_OUT_Play(out_buf, SAMPLE_OUT_BUFFER_SIZE);
	Sample_Descriptor.sample_flag[0] = SAMPLE_ACTIVE_BIT;
	Sample_Descriptor.sample_flag[1] = SAMPLE_ACTIVE_BIT;
	Sample_Descriptor.sample_flag[2] = SAMPLE_ACTIVE_BIT;
	Sample_Descriptor.sample_flag[3] = SAMPLE_ACTIVE_BIT;
	Sample_Descriptor.sample_flag[4] = SAMPLE_ACTIVE_BIT;
	Sample_Descriptor.sample_flag[5] = SAMPLE_ACTIVE_BIT;
	Sample_Descriptor.sample_flag[6] = SAMPLE_ACTIVE_BIT;
	Sample_Descriptor.sample_flag[7] = SAMPLE_ACTIVE_BIT;
	return 0;
}

static void store_to_outbuf(uint16_t buf_index ,uint16_t limitlow , uint16_t limithigh )
{
uint32_t	i;
uint16_t	sample;
	for(i=limitlow;i<limithigh;i++)
	{
		if ( buf_index == 0 )
		{
			out_buf[i] = file_buf[0][i];
		}
		else
		{
			sample = (out_buf[i] >> 1 ) + (file_buf[buf_index][i] >> 1);
			out_buf[i] = sample;
		}
	}
}

uint32_t	delay = 0;
static void SamplePlayerProcess(void)
{
uint32_t 	bytesread;
uint32_t	i = 0;

	HAL_GPIO_WritePin(ARD_D8_GPIO_GPIO_Port, ARD_D8_GPIO_Pin, GPIO_PIN_SET);
	if (( Sample_Descriptor.flag &  WAVSAMPLE_STATE_FLAG_HALF ) == WAVSAMPLE_STATE_FLAG_HALF)
	{
		for(i=0;i<MAX_NUM_SAMPLES;i++)
		{
			if (( Sample_Descriptor.sample_flag[i] &  SAMPLE_ACTIVE_BIT ) == SAMPLE_STATE_ACTIVE)
			{
				if(f_read(&WavFile[i],	(uint8_t *)&file_buf[i][0],	SAMPLE_OUT_BUFFER_SIZE,	(void *)&bytesread) == FR_OK)
				{
					store_to_outbuf(i,0,SAMPLE_OUT_HALFBUFFER_SIZE);
					Sample_Descriptor.fptr[i] += bytesread;
				}
				else
					Sample_Descriptor.sample_flag[i] &= ~SAMPLE_ACTIVE_BIT;
			}
		}
		Sample_Descriptor.flag &= ~WAVSAMPLE_STATE_FLAG_HALF;
	}
	if (( Sample_Descriptor.flag &  WAVSAMPLE_STATE_FLAG_FULL ) == WAVSAMPLE_STATE_FLAG_FULL)
	{
		for(i=0;i<MAX_NUM_SAMPLES;i++)
		{
			if (( Sample_Descriptor.sample_flag[i] &  SAMPLE_ACTIVE_BIT ) == SAMPLE_STATE_ACTIVE)
			{
				if(f_read(&WavFile[i],	(uint8_t *)&file_buf[i][SAMPLE_OUT_HALFBUFFER_SIZE],SAMPLE_OUT_BUFFER_SIZE,(void *)&bytesread) == FR_OK)
				{
					store_to_outbuf(i,SAMPLE_OUT_HALFBUFFER_SIZE,SAMPLE_OUT_BUFFER_SIZE);
					Sample_Descriptor.fptr[i] += bytesread;
				}
				else
					Sample_Descriptor.sample_flag[i] &= ~SAMPLE_ACTIVE_BIT;
			}
		}
		Sample_Descriptor.flag &= ~WAVSAMPLE_STATE_FLAG_FULL;
	}
	HAL_GPIO_WritePin(ARD_D8_GPIO_GPIO_Port, ARD_D8_GPIO_Pin, GPIO_PIN_RESET);
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
	/* allows AUDIO_Process() to refill 2nd part of the buffer  */
	Sample_Descriptor.flag |= WAVSAMPLE_STATE_FLAG_FULL;
	SamplePlayerProcess();
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
	/* allows AUDIO_Process() to refill 1st part of the buffer  */
	Sample_Descriptor.flag |= WAVSAMPLE_STATE_FLAG_HALF;
	SamplePlayerProcess();
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
