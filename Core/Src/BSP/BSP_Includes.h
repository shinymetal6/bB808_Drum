/*
 * BSP_Includes.h
 *
 *  Created on: Jul 27, 2021
 *      Author: fil
 */

#ifndef SRC_BSP_BSP_INCLUDES_H_
#define SRC_BSP_BSP_INCLUDES_H_

#include "stm32f723e_discovery.h"
#include "stm32f723e_discovery_audio.h"
#include "stm32f723e_discovery_lcd.h"
#include "stm32f723e_discovery_psram.h"
#include "stm32f723e_discovery_qspi.h"
#include "stm32f723e_discovery_ts.h"

extern	void tim1sec_callback(void);
extern	void InitLCD(char *title);

#define LCD_UsrLogY(...)	LCD_LineColor = LCD_COLOR_YELLOW;\
							printf(__VA_ARGS__);

#endif /* SRC_BSP_BSP_INCLUDES_H_ */
