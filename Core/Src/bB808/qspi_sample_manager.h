/*
 * qspi_sample_manager.h
 *
 *  Created on: Sep 17, 2021
 *      Author: fil
 */

#ifndef SRC_BB808_QSPI_SAMPLE_MANAGER_H_
#define SRC_BB808_QSPI_SAMPLE_MANAGER_H_

#ifdef QSPISAMPLEPLAYER

#define AUDIO_OUT_BUFFER_SIZE                      4096
#define AUDIO_OUT_HALFBUFFER_SIZE                  (AUDIO_OUT_BUFFER_SIZE/2)

#define	WAVPLAY_STATE_FLAG_HALF		1
#define	WAVPLAY_STATE_FLAG_FULL		2
#define	QSPI_SIZE					(1024*1024*8)
#define	INSTRUMENT_SIZE				(1024*1024)
#define	SECTOR64K					65536
#define	SECTOR4K					4096
#define	NUMSECTOR64K_PER_INSTRUMENT	(INSTRUMENT_SIZE / 65536)
#define	NUMSECTOR4K_PER_INSTRUMENT	(INSTRUMENT_SIZE / 4096)

#define	NUM_INSTRUMENT				(QSPI_SIZE / INSTRUMENT_SIZE)
//#define	NUMSECTORS					(QSPI_SIZE / SECTOR64K)
#define	HEADER_SIZE					32
#define	SAMPLE_NAME_MAX_LEN			20

/* Audio buffer control struct */
typedef struct {
	uint32_t		qspi_ptr[NUM_INSTRUMENT];
	uint32_t		sample_len[NUM_INSTRUMENT];
	uint32_t		sample_flag[NUM_INSTRUMENT];
	uint8_t			sample_name[NUM_INSTRUMENT][SAMPLE_NAME_MAX_LEN];	// max file len is SAMPLE_NAME_MAX_LEN
	uint32_t 		sample_rate[NUM_INSTRUMENT];
	uint8_t			midi_key[NUM_INSTRUMENT];
	uint8_t			flag;
}Instrument_TypeDef;
/* sample_flag defines */
#define	SAMPLE_ACTIVE_BIT	1

typedef struct {
  uint32_t ChunkID;       /* 0 */
  uint32_t FileSize;      /* 4 */
  uint32_t FileFormat;    /* 8 */
  uint32_t SubChunk1ID;   /* 12 */
  uint32_t SubChunk1Size; /* 16*/
  uint16_t AudioFormat;   /* 20 */
  uint16_t NbrChannels;   /* 22 */
  uint32_t SampleRate;    /* 24 */

  uint32_t ByteRate;      /* 28 */
  uint16_t BlockAlign;    /* 32 */
  uint16_t BitPerSample;  /* 34 */
  uint32_t SubChunk2ID;   /* 36 */
  uint32_t SubChunk2Size; /* 40 */
}WAVE_FormatTypeDef;

typedef struct {
	uint8_t		wav_name[SAMPLE_NAME_MAX_LEN];	// max file len is SAMPLE_NAME_MAX_LEN
	uint32_t	sample_rate;
	uint32_t	wav_len;
	uint8_t		empty0;
	uint8_t		empty1;
	uint8_t		empty2;
	uint8_t		midi_key;
}QSPISample_HeaderTypeDef;


extern	void QSPISamplePlayerInit(void);
extern	void QSPISamplePlayerStart(void);
extern	void QSPIInstrumentON(uint8_t instrument_number);

#endif /* #ifdef QSPISAMPLEPLAYER */
#endif /* SRC_BB808_QSPI_SAMPLE_MANAGER_H_ */
