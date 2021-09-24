/*
 * beat_indicator.h
 *
 *  Created on: Sep 18, 2021
 *      Author: fil
 */

#ifndef SRC_BB808_BPM_H_
#define SRC_BB808_BPM_H_

#define MAX_BEAT	240

extern	void BPM_Draw(uint8_t hilight);
extern	void BPM_IncDec(void);
extern	void BPM_Init(void);

#endif /* SRC_BB808_BPM_H_ */
