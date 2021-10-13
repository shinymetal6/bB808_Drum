/*
 * bB808.h
 *
 *  Created on: Sep 13, 2021
 *      Author: fil
 */

#ifndef SRC_BB808_BB808_H_
#define SRC_BB808_BB808_H_

#include "main.h"
#include "delay_line.h"
#include "menus.h"
#include "usb_sample_mngr.h"
#include "qspi_sample_manager.h"
#include "qspi_sample_store.h"
#include "sequencer.h"
#include "bpm.h"

typedef struct {
	uint8_t			system;
	uint8_t			sequencer;
	uint32_t		sequencer_counter;
	uint32_t		sequencer_preload;
	uint16_t		sequencer_length;
	uint16_t		sequencer_step;
	uint8_t			menu_state;
	uint8_t			next_menu_item;
	uint8_t			selected_menu_item;
	Menu_TypeDef 	*current_menu;
	uint32_t 		encval;
	uint32_t		last_encval;
	uint8_t 		encoder_flag;
	uint8_t			timers_flag;
	uint8_t			tim100msec_counter;
	uint8_t			tim1Sec_counter;
	uint8_t			DrawBitIndicator_counter;
	uint16_t		beat;
	uint16_t		delay;
	uint8_t			delay_type;
	uint16_t		delay_insertion_pointer;
	uint16_t		delay_weight_int;
	float			delay_weight;

}SystemVar_TypeDef;

/* system flag */
/* SYSTEM_INTEXT_SEQUENCER = 0 : external sequencer */
#define	SYSTEM_INTEXT_SEQUENCER		0x01
#define	SYSTEM_BPM_INCDEC			0x02
#define	SYSTEM_DELAYVAL_INCDEC		0x04
#define	SYSTEM_DELAYWEIGHT_INCDEC	0x08
#define	SYSTEM_MENU_INCDEC			0x10
#define	SYSTEM_ERASE_IN_PROGRESS	0x20
#define	SYSTEM_USB_INIT				0x40
#define	SYSTEM_MIDIDEV_FLAG			0x80

#define	INTERNAL_SEQUENCER			SYSTEM_INTEXT_SEQUENCER
#define	EXTERNAL_SEQUENCER			0

/* sequencer flags */
#define	SEQUENCER_TICK				0x01
#define	SEQUENCER_PRELOAD			0x02
#define	SEQUENCER_SINGLE			0x40
#define	SEQUENCER_IRQ_FLAG			0x80

/* timers_flag */
#define	TIMER_100MS_FLAG			0x01
#define	TIMER_1SEC_FLAG				0x02

/* encoder_flag */
#define	ENCODER_ROTATION_FLAG		0x01
#define	ENCODER_SW_FLAG				0x02

/* delay_type */
#define	DELAY_TYPE_NONE			0x00
#define	DELAY_TYPE_ECHO			0x01
#define	DELAY_TYPE_FLANGER		0x02

#define	LCD_RESOLUTION_X	240
#define	LCD_RESOLUTION_Y	240
#define DIGIT_W    20
#define DIGIT_H    36

extern	SystemVar_TypeDef	SystemVar;
extern	const uint8_t digits[10][1578];

extern	void bB808_Init(void);
extern	void bB808_Loop(void);
extern	void ReadDescriptorFileFromUSB(uint8_t line_idx);
extern	void ClearDescriptorFileArea(uint8_t x , uint8_t y);
extern	void tim100msec_callback(void);
extern	void encoder_rotation_callback(void);
extern	void encoder_sw_callback(void);

extern const uint8_t green_digits[10][1578];
extern const uint8_t red_digits[10][1578];
extern const uint8_t blue_digits[10][1578];
extern const uint8_t beat[2730];

#endif /* SRC_BB808_BB808_H_ */
