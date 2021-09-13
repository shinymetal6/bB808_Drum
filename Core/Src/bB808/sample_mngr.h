/*
 * sample_mngr.h
 *
 *  Created on: Sep 13, 2021
 *      Author: fil
 */

#ifndef SRC_BB808_SAMPLE_MNGR_H_
#define SRC_BB808_SAMPLE_MNGR_H_


#define	MAX_NUM_SAMPLES				16
#define SAMPLE_OUT_BUFFER_SIZE		2048
#define SAMPLE_OUT_HALFBUFFER_SIZE 	(SAMPLE_OUT_BUFFER_SIZE/2)
#define	SAMPLE_NAME_MAX_LEN			32
typedef struct {
	uint32_t	fptr[MAX_NUM_SAMPLES];
	uint32_t	sample_name[MAX_NUM_SAMPLES][SAMPLE_NAME_MAX_LEN];	// max file len is SAMPLE_NAME_MAX_LEN
	uint8_t		flag;
}Sample_DescriptorTypeDef;

/* flag defines */
#define	WAVSAMPLE_STATE_FLAG_HALF		1
#define	WAVSAMPLE_STATE_FLAG_FULL		2

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

#endif /* SRC_BB808_SAMPLE_MNGR_H_ */
