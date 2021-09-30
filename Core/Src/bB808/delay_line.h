/*
 * delay_line.h
 *
 *  Created on: Jul 1, 2021
 *      Author: fil
 */

#ifndef INC_BB101_VCO_COMPONENTS_EFFECTS_DELAY_LINE_H_
#define INC_BB101_VCO_COMPONENTS_EFFECTS_DELAY_LINE_H_

#define	DELAY_LINE_SIZE		(65536)
#define	DELAY_LINE_MULT		(DELAY_LINE_SIZE/128)
/*
#define	DELAY_TYPE_FLANGER	0
#define	DELAY_TYPE_REVERB	1
*/
#define	MAX_DELAY	999
#define	MAX_DELAYW	90
#define	MIN_DELAYW	10

extern	int16_t DelayLine(int16_t sample ,  uint8_t delay_type);
extern	void DelayLineInit(void);
extern	void Delay_Draw(uint8_t hilight);
extern	void Delay_Weight_Draw(uint8_t hilight);
extern	void Delay_IncDec(void);
extern	void Delay_Weight_IncDec(void);
extern	void DelayTypeDisplay(void);

#endif /* INC_BB101_VCO_COMPONENTS_EFFECTS_DELAY_LINE_H_ */
