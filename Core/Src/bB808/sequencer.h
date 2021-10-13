/*
 * sequencer.h
 *
 *  Created on: Sep 24, 2021
 *      Author: fil
 */

#ifndef SRC_BB808_SEQUENCER_H_
#define SRC_BB808_SEQUENCER_H_

#define	SEQUENCER_MAX_SIZE	1024
#define	HEADER_LEN			8

extern	void SequencerInit(void);
extern	void SequencerCallback(void);

typedef struct {
	uint8_t		header[8];
	uint16_t	sequencer_length;
	uint16_t	sequencer_beat;
	uint16_t	delay_type;
	uint16_t	delay;
	uint16_t	delay_weight;
	uint16_t	flag;
	uint16_t	sequencer_steps[SEQUENCER_MAX_SIZE];
}Sequencer_DescriptorTypeDef;

//extern	void Sequencer(void);
//extern	void SequencerSet(void);

/* sequencer file example , simple 4/4 , 4 beat
hihat on 0
bassdrum on 1
snare on 2

I I I I I I I I I I I I I I
n n n n n n n n n n n n n n
s s s s s s s s s s s s s s
t t t t t t t t t t t t t t
r r r r r r r r r r r r r r
u u u u u u u u u u u u u u
m m m m m m m m m m m m m m
e e e e e e e e e e e e e e
n n n n n n n n n n n n n n
t t t t t t t t t t t t t t
0 1 2 3 4 5 6 7 8 9 a b c d

LOOPLEN	4
1 0 0 0 0 0 0 0 0 0 0 0 0 0
1 1 0 0 0 0 0 0 0 0 0 0 0 0
1 0 1 0 0 0 0 0 0 0 0 0 0 0
1 0 0 0 0 0 0 0 0 0 0 0 0 0
*/

extern	uint16_t sequencer_steps[SEQUENCER_MAX_SIZE];


#endif /* SRC_BB808_SEQUENCER_H_ */
