/*
 * FAT_Utils.c
 *
 *  Created on: Jul 27, 2021
 *      Author: fil
 */

#include "main.h"
#include "ff.h"

extern	USBH_HandleTypeDef hUSBHost;

static int32_t recurseLevel = -1;

FRESULT Explore_Disk(char *path, uint8_t recu_level)
{
  FRESULT res = FR_OK;
  FILINFO fno;
  DIR dir;
  char tmp[14];
  uint8_t line_idx = 0;

  recurseLevel++;
  res = f_opendir(&dir, path);
  if (res == FR_OK)
  {
    while (USBH_MSC_IsReady(&hUSBHost))
    {
      res = f_readdir(&dir, &fno);
      if (res != FR_OK || fno.fname[0] == 0)
      {
        break;
      }
      if (fno.fname[0] == '.')
      {
        continue;
      }

      strcpy(tmp, fno.fname);

      line_idx++;

#ifdef pippo
      if (line_idx > YWINDOW_SIZE)
      {
        line_idx = 0;
        LCD_UsrLog("> Press [Key] To Continue.\n");

        /* KEY Button in polling */
        while ((BSP_PB_GetState(BUTTON_USER) != SET) &&
               (Appli_state != APPLICATION_DISCONNECT))
        {
          /* Wait for User Input */
        }
      }
#endif
/*
      if (recu_level == 1)
      {
        LCD_DbgLog("   |__");
      }
      else if (recu_level == 2)
      {
        LCD_DbgLog("   |   |__");
      }
      */
      if (fno.fattrib & AM_DIR)
      {
        strcat(tmp, "\n");
        LCD_UsrLog((void *)tmp);
        Explore_Disk(fno.fname, 2);
      }
      else
      {
        strcat(tmp, "\n");
        LCD_UsrLog((void *)tmp);
      }

      if ((fno.fattrib & AM_DIR) && (recu_level == 2))
      {
        Explore_Disk(fno.fname, 2);
      }
    }
    f_closedir(&dir);
  }

  if (--recurseLevel == -1)
  {
    LCD_UsrLog("> Select an operation to Continue.\n");
  }

  return res;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
