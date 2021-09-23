/*
 * menus.h
 *
 *  Created on: Sep 20, 2021
 *      Author: fil
 */

#ifndef SRC_BB808_MENUS_H_
#define SRC_BB808_MENUS_H_

#define	MENU_FONT_HEIGHT		15
#define	MENU_LINE_MAX_LEN		32
#define	MENU_LINE_0_X			1
#define	MENU_LINE_0_Y			45
#define	MENU_INACTIVE_COLOR		LCD_COLOR_WHITE
#define	MENU_ACTIVE_COLOR		LCD_COLOR_LIGHTBLUE
#define	MENU_DELETE_COLOR		LCD_COLOR_BLACK
#define	MENU_MAXLINENUM			12

#define	MENU_USBKEY_ICON_W		24
#define	MENU_USBKEY_ICON_H		24
#define	MENU_USBKEY_ICON_X		(LCD_RESOLUTION_X-MENU_USBKEY_ICON_W)
#define	MENU_USBKEY_ICON_Y		(LCD_RESOLUTION_Y-MENU_USBKEY_ICON_H)


#define	BPM_DIGIT_XPOSU			(MENU_USBKEY_ICON_X-DIGIT_W-4)
#define	BPM_DIGIT_XPOST			(BPM_DIGIT_XPOSU-DIGIT_W-1)
#define	BPM_DIGIT_XPOSH			(BPM_DIGIT_XPOST-DIGIT_W-1)
#define	BPM_DIGIT_YPOS			(LCD_RESOLUTION_Y-DIGIT_H)

#define	MENU_BEAT_ICON_W		36
#define	MENU_BEAT_ICON_H		36
#define	MENU_BEAT_ICON_X		(BPM_DIGIT_XPOSH-MENU_BEAT_ICON_W - 4 )
#define	MENU_BEAT_ICON_Y		(LCD_RESOLUTION_Y-MENU_BEAT_ICON_H)

#define	DLY_DIGIT_XPOSU			(MENU_BEAT_ICON_X-DIGIT_W-4)
#define	DLY_DIGIT_XPOST			(DLY_DIGIT_XPOSU-DIGIT_W-1)
#define	DLY_DIGIT_XPOSH			(DLY_DIGIT_XPOST-DIGIT_W-1)
#define	DLY_DIGIT_YPOS			(LCD_RESOLUTION_Y-DIGIT_H)


typedef struct {
	uint8_t			linex,liney;
	uint8_t			text[MENU_LINE_MAX_LEN];
	uint16_t		color;
	uint8_t			items;
	uint8_t			state_next_index;
}Menu_TypeDef;


/* states */
typedef enum
{
	  MENU_TOP,
	  MENU_SAMPLES,
	  MENU_SEQUENCE,
	  MENU_DELAY,
	  MENU_SETTINGS
}MenuState_Typdef;

extern	uint32_t 	encoder_flag;
extern	void   encoder_callback(void);
extern	void MenuDisplayInit(void);
extern	void MenuEncoderNavigate(void);
extern	void MeuEncoderChangeMenu(void);

#endif /* SRC_BB808_MENUS_H_ */
