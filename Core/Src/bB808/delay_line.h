/*
 * delay_line.h
 *
 *  Created on: Jul 1, 2021
 *      Author: fil
 */

#ifndef INC_BB101_VCO_COMPONENTS_EFFECTS_DELAY_LINE_H_
#define INC_BB101_VCO_COMPONENTS_EFFECTS_DELAY_LINE_H_

#define	DELAY_LINE_SIZE		(65536)
#define	DELAY_LINE_MULT		(512)
#define	DELAY_TYPE_FLANGER	0
#define	DELAY_TYPE_REVERB	1

extern	float		delay_weight;
extern	uint32_t	delay_insertion_pointer,delay_extraction_pointer;
extern	uint8_t		control_message, program_message;


extern	int16_t DelayLine(int16_t sample ,  uint8_t delay_type);

#endif /* INC_BB101_VCO_COMPONENTS_EFFECTS_DELAY_LINE_H_ */
