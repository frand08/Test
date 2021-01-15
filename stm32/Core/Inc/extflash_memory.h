/**
  ******************************************************************************
  * @file extflash_memory.h
  * @author fdominguez
  * @brief This file provides code for specific FLASH application
  * @date 01/13/2020
  * @version 1.0.0
  ******************************************************************************
  */
#ifndef FLASH_MEMORY_H_
#define FLASH_MEMORY_H_

#include <stdint.h>

/* FIXME: 4096 (sector size) / 12 does not result in an integer, so certain data
 * would be stored in two different sectors */
/* ID + x_mag + y_mag + z_mag + temp = 12 bytes */
#define EXTFLASH_BYTES_STORED_PER_FIELD	12
#define EXTFLASH_BYTES_TO_READ_DATA		(EXTFLASH_BYTES_STORED_PER_FIELD - 4)
#define EXTFLASH_MAX_WRITE_RETRIALS		4

typedef enum
{
  EXTFLASH_ERROR = -1,
  EXTFLASH_OK    = 0
} EXTFLASH_StatusTypeDef;

EXTFLASH_StatusTypeDef EXTFLASH_CheckID(uint32_t id_value);
EXTFLASH_StatusTypeDef EXTFLASH_ReadData(uint32_t id_value, int16_t *x_mag, int16_t *y_mag, int16_t *z_mag, int16_t *temp);
EXTFLASH_StatusTypeDef EXTFLASH_WriteData(uint32_t id_value, int16_t x_mag, int16_t y_mag, int16_t z_mag, int16_t temp);

#endif /* FLASH_MEMORY_H_ */
