/*
 * beat_indicator.h
 *
 *  Created on: Sep 18, 2021
 *      Author: fil
 */

#ifndef SRC_BB808_BPM_H_
#define SRC_BB808_BPM_H_

#define MAX_BEAT	175
/* value for MAX_BEAT */
#define	BPM_UNIT	334
/* value for 1 BPM */
#define	BPM_1_BPM	60120
/* Logic : write in counter register the value (BPM_1_BPM - (<bpm> * BPM_UNIT) where <bpm> must be less than MAX_BEAT */
#define	BPM_60_BPM	(60*BPM_UNIT)


extern	void BPM_Draw(uint8_t hilight);
extern	void BPM_IncDec(void);
extern	void BPM_Init(void);

#endif /* SRC_BB808_BPM_H_ */
