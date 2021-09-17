/*
 * qspi_sample_manager.c
 *
 *  Created on: Sep 16, 2021
 *      Author: fil
 */

#include "main.h"
#include "qspi_sample_manager.h"
#include "fatfs.h"

#ifdef QSPISAMPLEPLAYER

static Instrument_TypeDef  Instrument;
static uint32_t uwVolume = 70;
__attribute__ ((aligned (16)))  uint16_t file_buf[NUM_INSTRUMENT][AUDIO_OUT_HALFBUFFER_SIZE];
__attribute__ ((aligned (16)))  uint16_t out_buf[AUDIO_OUT_BUFFER_SIZE];
__attribute__ ((aligned (16)))  QSPISample_HeaderTypeDef QSPISample_Header[NUM_INSTRUMENT]; //,ReadSample_Header[NUM_INSTRUMENT];

#define	FROM_QSPI	1

void QSPISamplePlayerInit(void)
{
	BSP_QSPI_Init();
	HAL_Delay(100);
}

void QSPISamplePlayerStart(void)
{
uint32_t		instrument_number = 0;
uint32_t		instrument_address = 0;
uint32_t		i;

	for(instrument_number=0;instrument_number<NUM_INSTRUMENT;instrument_number++)
	{
		instrument_address = instrument_number * INSTRUMENT_SIZE;
		BSP_QSPI_Read((uint8_t *)&QSPISample_Header[instrument_number], instrument_address, HEADER_SIZE);
		Instrument.qspi_ptr[instrument_number] = HEADER_SIZE;
		Instrument.sample_len[instrument_number] = QSPISample_Header[instrument_number].wav_len;
		Instrument.sample_rate[instrument_number] = QSPISample_Header[instrument_number].sample_rate;
		Instrument.midi_key[instrument_number] = QSPISample_Header[instrument_number].midi_key;
		for(i=0;i<SAMPLE_NAME_MAX_LEN;i++)
			Instrument.sample_name[instrument_number][i] = QSPISample_Header[instrument_number].wav_name[i];
		for(i=0;i<AUDIO_OUT_HALFBUFFER_SIZE;i++)
			file_buf[instrument_number][i] = 0;
	}
	if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_BOTH, uwVolume, Instrument.sample_rate[0]) != 0) // get sample rate from first sample
		return;
	BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
	BSP_AUDIO_OUT_Play(out_buf, AUDIO_OUT_BUFFER_SIZE);
	return;
}

void Sample_Process(void)
{
uint32_t 	i,limitlow,limithi;
uint8_t		instrument_number = 0;
uint32_t	sample;

	HAL_GPIO_WritePin(ARD_D8_GPIO_GPIO_Port, ARD_D8_GPIO_Pin, GPIO_PIN_SET);
	__disable_irq();

	if (( Instrument.flag &  WAVPLAY_STATE_FLAG_HALF ) == WAVPLAY_STATE_FLAG_HALF)
	{
		limitlow = 0;
		limithi = AUDIO_OUT_HALFBUFFER_SIZE;
	}

	if (( Instrument.flag &  WAVPLAY_STATE_FLAG_FULL ) == WAVPLAY_STATE_FLAG_FULL)
	{
		limitlow = AUDIO_OUT_HALFBUFFER_SIZE;
		limithi = AUDIO_OUT_BUFFER_SIZE;
	}

	for(instrument_number=0;instrument_number<NUM_INSTRUMENT;instrument_number++)
	{
		if ( Instrument.sample_flag[instrument_number] != 0 )
		{

			if(Instrument.qspi_ptr[instrument_number] >= Instrument.sample_len[instrument_number])
			{
				for(i=limitlow;i<limithi;i++)
					file_buf[instrument_number][i-limitlow] = 0;
				Instrument.sample_flag[instrument_number] = 0;
				Instrument.qspi_ptr[instrument_number] = HEADER_SIZE;
			}
			else
			{
				BSP_QSPI_Read((uint8_t *)file_buf[instrument_number], INSTRUMENT_SIZE*instrument_number + Instrument.qspi_ptr[instrument_number], AUDIO_OUT_BUFFER_SIZE);
				Instrument.qspi_ptr[instrument_number] += AUDIO_OUT_BUFFER_SIZE;
			}
		}
	}
	Instrument.flag &= ~WAVPLAY_STATE_FLAG_HALF;
	Instrument.flag &= ~WAVPLAY_STATE_FLAG_FULL;

	for(i=limitlow;i<limithi;i++)
	{
		sample =	__REVSH(file_buf[0][i-limitlow] )+
					__REVSH(file_buf[1][i-limitlow]) +
					__REVSH(file_buf[2][i-limitlow]) +
					__REVSH(file_buf[3][i-limitlow]) +
					__REVSH(file_buf[4][i-limitlow]) +
					__REVSH(file_buf[5][i-limitlow]) +
					__REVSH(file_buf[6][i-limitlow]) +
					__REVSH(file_buf[7][i-limitlow]);

		sample >>= 4;
		out_buf[i] = (uint16_t )sample;

	}

	__enable_irq();
	HAL_GPIO_WritePin(ARD_D8_GPIO_GPIO_Port, ARD_D8_GPIO_Pin, GPIO_PIN_RESET);
	return ;
}


void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
	/* allows AUDIO_Process() to refill 2nd part of the buffer  */
	Instrument.flag |= WAVPLAY_STATE_FLAG_FULL;
	Sample_Process();
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
	/* allows AUDIO_Process() to refill 1st part of the buffer  */
	Instrument.flag |= WAVPLAY_STATE_FLAG_HALF;
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


void QSPIInstrumentON(uint8_t instrument_number)
{
uint32_t	i;
	for(i=0;i<NUM_INSTRUMENT;i++)
	{
		if ( Instrument.midi_key[i] == instrument_number)
		{
			Instrument.qspi_ptr[i] = HEADER_SIZE;
			Instrument.sample_flag[i] |=  SAMPLE_ACTIVE_BIT;
			return;
		}
	}
}

void QSPIInstrumentOFF(uint8_t instrument_number)
{
	//QSPISample_Descriptor.sample_flag[instrument_number] &= ~SAMPLE_ACTIVE_BIT;
}

uint8_t	qspi_midi_buffer[64],qspi_midi_rxbuffer[64];
extern	USBH_StatusTypeDef  USBH_MIDI_Receive(USBH_HandleTypeDef *phost, uint8_t *pbuff, uint16_t length);
extern	uint32_t	mididev_flag;
USBH_HandleTypeDef *midi_phost;

void restart_midi(void)
{
	USBH_MIDI_Receive(midi_phost, qspi_midi_rxbuffer, 64);
}

void USBH_MIDI_ReceiveCallback(USBH_HandleTypeDef *phost)
{
uint8_t	i,k=0,j;

	midi_phost = phost;
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
	if (qspi_midi_rxbuffer[0] == 0x09 )
		QSPIInstrumentON(qspi_midi_buffer[2]);
	if (qspi_midi_rxbuffer[0] == 0x08 )
		QSPIInstrumentOFF(qspi_midi_buffer[2]);
	mididev_flag = 1;

	//USBH_MIDI_Receive(midi_phost, qspi_midi_rxbuffer, 64);
}

#endif
