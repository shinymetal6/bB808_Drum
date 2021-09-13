/*
 * sineplay.h
 *
 *  Created on: Aug 12, 2021
 *      Author: fil
 */

#ifndef SRC_BSP_SUBSYSTEM_AUDIO_SINEPLAY_H_
#define SRC_BSP_SUBSYSTEM_AUDIO_SINEPLAY_H_

#define SINEPLAY_DEFAULT_VOLUME    	70
#define SINEPLAY_DEFAULT_SAMPLEFREQ    44100
#define AUDIO_OUT_BUFFER_SIZE_INT16			2048
#define AUDIO_OUT_BUFFER_SIZE_INT8			(AUDIO_OUT_BUFFER_SIZE_INT16*2)
#define AUDIO_OUT_BUFFER_SIZE_HALF_INT16	(AUDIO_OUT_BUFFER_SIZE_INT16/2)
#define AUDIO_OUT_BUFFER_SIZE_HALF_INT8		(AUDIO_OUT_BUFFER_SIZE_INT8/2)

#define AUDIO_OUT_HALFBUFFER_SIZE                  (AUDIO_OUT_BUFFER_SIZE/2)

typedef struct {
	uint8_t 	flag;
	uint8_t		Volume;
	uint32_t	AudioFreq;	//11500 , 12525, 16000, 22050, 32000, 44115, 48000, 96000
	uint32_t	srcbuf_index;
	uint32_t	srcbuf_size;
}AUDIO_BufferTypeDef;

typedef struct {
  uint8_t buff[AUDIO_OUT_BUFFER_SIZE_INT8];
  uint32_t fptr;
  uint8_t	flag;
}AUDIO_OUT_BufferTypeDef;

extern AUDIO_BufferTypeDef  Audio;

#define	SINEPLAY_STATE_FLAG_HALF		1
#define	SINEPLAY_STATE_FLAG_FULL		2


extern	uint16_t audio_sample[AUDIO_OUT_BUFFER_SIZE_INT16];


#endif /* SRC_BSP_SUBSYSTEM_AUDIO_SINEPLAY_H_ */
