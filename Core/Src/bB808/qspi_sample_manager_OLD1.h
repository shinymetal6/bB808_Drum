/*
 * qspi_sample_manager.h
 *
 *  Created on: Sep 16, 2021
 *      Author: fil
 */

#ifndef SRC_BB808_QSPI_SAMPLE_MANAGER_OLD1_H_
#define SRC_BB808_QSPI_SAMPLE_MANAGER_OLD1_H_

#ifdef QSPISAMPLEPLAYER

#define	QSPI_SIZE					(1024*1024*8)
#define	SECTOR64K					65536
#define	NUMSECTOR_PER_INSTRUMENT	16
#define	INSTRUMENT_SIZE				(SECTOR64K*NUMSECTOR_PER_INSTRUMENT)
#define	NUM_INSTRUMENT				(QSPI_SIZE / INSTRUMENT_SIZE)
#define	NUMSECTORS					(QSPI_SIZE / SECTOR64K)
#define	HEADER_SIZE					32
#define	SAMPLE_NAME_MAX_LEN			24
#define SAMPLE_OUT_BUFFER_SIZE		1024
#define	SAMPLE_OUT_HALFBUFFER_SIZE	(SAMPLE_OUT_BUFFER_SIZE/2)


typedef struct {
	uint8_t		wav_name[20];	// max file len is SAMPLE_NAME_MAX_LEN
	uint32_t	sample_rate;
	uint32_t	wav_len;
	uint8_t		empty0;
	uint8_t		empty1;
	uint8_t		empty2;
	uint8_t		midi_key;
}QSPISample_HeaderTypeDef;

typedef struct {
	uint32_t	qspi_ptr[NUM_INSTRUMENT];
	uint32_t	sample_len[NUM_INSTRUMENT];
	uint32_t	sample_flag[NUM_INSTRUMENT];
	uint8_t		sample_name[NUM_INSTRUMENT][SAMPLE_NAME_MAX_LEN];	// max file len is SAMPLE_NAME_MAX_LEN
	uint32_t 	sample_rate[NUM_INSTRUMENT];
	uint8_t		midi_key[NUM_INSTRUMENT];
	uint8_t		flag;
}QSPISample_DescriptorTypeDef;

/* flag defines */
#define	QSPISAMPLE_STATE_FLAG_HALF		1
#define	QSPISAMPLE_STATE_FLAG_FULL		2
/* sample_flag defines ( for each instrument ) */
#define	SAMPLE_STATE_INACTIVE			0
#define	SAMPLE_STATE_ACTIVE				1
#define	SAMPLE_ACTIVE_BIT				1


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
}SampleWAV_FormatTypeDef;
#endif
#endif /* SRC_BB808_QSPI_SAMPLE_MANAGER_OLD1_H_ */
