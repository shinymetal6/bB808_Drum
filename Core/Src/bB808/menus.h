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
#define	MENU_LINE_0_Y			26
#define	MENU_INACTIVE_COLOR		LCD_COLOR_WHITE
#define	MENU_ACTIVE_COLOR		LCD_COLOR_LIGHTBLUE
#define	MENU_DELETE_COLOR		LCD_COLOR_BLACK
#define	MENU_MAXLINENUM			12

#define	MENU_USBKEY_ICON_W		24
#define	MENU_USBKEY_ICON_H		24
#define	MENU_USBKEY_ICON_X		(LCD_RESOLUTION_X-MENU_USBKEY_ICON_W)
#define	MENU_USBKEY_ICON_Y		(2)

#define DIGIT_X    20
#define DIGIT_Y    36

#define	BPM_DIGIT_XPOSU			(LCD_RESOLUTION_X-DIGIT_X)
#define	BPM_DIGIT_XPOST			(BPM_DIGIT_XPOSU-DIGIT_W-1)
#define	BPM_DIGIT_XPOSH			(BPM_DIGIT_XPOST-DIGIT_W-1)
#define	BPM_DIGIT_YPOS			(MENU_USBKEY_ICON_Y+MENU_USBKEY_ICON_H+4)

#define	BPM_TEXT_X				(BPM_DIGIT_XPOSH - 36 )
#define	BPM_TEXT_Y				(BPM_DIGIT_YPOS+(DIGIT_Y / 2))

#define	DLY_DIGIT_XPOSU			(LCD_RESOLUTION_X-DIGIT_X)
#define	DLY_DIGIT_XPOST			(DLY_DIGIT_XPOSU-DIGIT_W-1)
#define	DLY_DIGIT_XPOSH			(DLY_DIGIT_XPOST-DIGIT_W-1)
#define	DLY_DIGIT_YPOS			(BPM_DIGIT_YPOS+DIGIT_Y+2)

#define	DLY_TEXT_X				(DLY_DIGIT_XPOSH - 46 )
#define	DLY_TEXT_Y				(DLY_DIGIT_YPOS+(DIGIT_Y / 2))

#define	DESCRIPTOR_AREA_X		0
#define	DESCRIPTOR_AREA_Y		DLY_DIGIT_YPOS+DIGIT_Y

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

extern	void MenuDisplayInit(void);
extern	void MenuEncoderNavigate(void);
extern	void MeuEncoderChangeMenu(void);

#endif /* SRC_BB808_MENUS_H_ */
