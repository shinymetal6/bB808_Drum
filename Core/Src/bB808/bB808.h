/*
 * bB808.h
 *
 *  Created on: Sep 13, 2021
 *      Author: fil
 */

#ifndef SRC_BB808_BB808_H_
#define SRC_BB808_BB808_H_

#include "main.h"
#include "qspi_sample_manager.h"
#include "delay_line.h"
#include "menus.h"
#include "beat_indicator.h"
#include "usb_sample_mngr.h"

typedef struct {
	uint8_t			system;
	uint8_t			audio_init_flag;
	uint8_t			usbdisk_ready;
	uint8_t			menu_state;
	uint8_t			next_menu_item;
	uint8_t			selected_menu_item;
	Menu_TypeDef 	*current_menu;
	uint32_t 		encval;
	uint32_t		last_encval;
	uint8_t 		encoder_flag;
	uint8_t 		sw_disable;
	uint8_t			tim50msec_flag;
	uint8_t			tim100msec_flag;
	uint8_t			DrawBitIndicator_counter;
	uint16_t		beat;
	uint16_t		delay;
	uint8_t			delay_type;
	uint32_t		delay_insertion_pointer;
	uint32_t		delay_extraction_pointer;
	float			delay_weight;

}SystemVar_TypeDef;

/* system flag */
#define	SYSTEM_INTERNAL_SEQUENCER	0x01
#define	SYSTEM_EXTERNAL_SEQUENCER	0x02
#define	SYSTEM_BPM_INCDEC			0x04
#define	SYSTEM_DELAYVAL_INCDEC		0x08
#define	SYSTEM_DELAYTYPE_INCDEC		0x10
#define	SYSTEM_MENU_INCDEC			0x40
#define	SYSTEM_MIDIDEV_FLAG			0x80

/* delay_type */
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
extern	void ReadDescriptorFileFromUSB(void);

extern const uint8_t green_digits[10][1578];
extern const uint8_t red_digits[10][1578];
extern const uint8_t blue_digits[10][1578];
extern const uint8_t beat[2730];

#endif /* SRC_BB808_BB808_H_ */
