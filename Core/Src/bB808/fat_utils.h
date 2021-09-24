/*
 * fat_utils.h
 *
 *  Created on: Sep 23, 2021
 *      Author: fil
 */

#ifndef SRC_BB808_FAT_UTILS_H_
#define SRC_BB808_FAT_UTILS_H_


extern	void ReadDescriptorFileFromUSB(void);
extern	FRESULT Explore_Disk(char *path);
extern	void ReadDescriptorFileFromUSB(void);
extern	void ClearDescriptorFileArea(uint8_t line_from);

#endif /* SRC_BB808_FAT_UTILS_H_ */
