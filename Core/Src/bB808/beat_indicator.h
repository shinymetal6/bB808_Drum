/*
 * beat_indicator.h
 *
 *  Created on: Sep 18, 2021
 *      Author: fil
 */

#ifndef SRC_BB808_BEAT_INDICATOR_H_
#define SRC_BB808_BEAT_INDICATOR_H_

#define MAX_BEAT	180

extern	void DrawBPM(uint8_t hilight);
extern	void DrawBPM_Icon(void);
extern	void DrawBitIndicator(uint8_t on);
extern	void BPM_IncDec(void);

#endif /* SRC_BB808_BEAT_INDICATOR_H_ */
