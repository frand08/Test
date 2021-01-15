/**
  ******************************************************************************
  * @file extflash_memory.c
  * @author fdominguez
  * @brief This file provides code for specific FLASH application
  * @date 01/13/2020
  * @version 1.0.0
  ******************************************************************************
  */

#include "extflash_memory.h"
#include "w25q80dv.h"
#include <stdlib.h>

/**
  * @brief Checks the ID is present on FLASH memory
  * @param id_value: ID to look for in FLASH memory
  * @return FLASH Status
  */
EXTFLASH_StatusTypeDef EXTFLASH_CheckID(uint32_t id_value)
{
  EXTFLASH_StatusTypeDef retval = EXTFLASH_ERROR;
  char aux[4];
  uint32_t high_index_aux = (id_value & (~(W25Q80DV_SECTOR_SIZE-1))) * W25Q80DV_SECTOR_SIZE;
  uint32_t low_index_aux = (id_value & (W25Q80DV_SECTOR_SIZE-1)) * EXTFLASH_BYTES_STORED_PER_FIELD;
  uint32_t starting_position =  high_index_aux + low_index_aux;

  /* Read ID in memory */
  if(W25Q80DV_ReadBytes(starting_position, (uint8_t*)aux, 4) == W25Q80DV_OK)
  {
	  /* Check if both ID matches */
	  if(id_value == atoi(aux))
		retval = EXTFLASH_OK;
  }

  return retval;
}

/**
  * @brief Reads data from external FLASH memory
  * @param id_value: ID to look for in FLASH memory
  * @param x_mag: magnetometer X value read
  * @param y_mag: magnetometer Y value read
  * @param z_mag: magnetometer Z value read
  * @param temp: temperature value read
  * @return EXTFLASH Status
  */
EXTFLASH_StatusTypeDef EXTFLASH_ReadData(uint32_t id_value, int16_t *x_mag, int16_t *y_mag, int16_t *z_mag, int16_t *temp)
{
  EXTFLASH_StatusTypeDef retval = EXTFLASH_ERROR;
  uint8_t aux[4];
  uint32_t high_index_aux = (id_value & (~(W25Q80DV_SECTOR_SIZE-1))) * W25Q80DV_SECTOR_SIZE;
  uint32_t low_index_aux = (id_value & (W25Q80DV_SECTOR_SIZE-1)) * EXTFLASH_BYTES_STORED_PER_FIELD + 4;
  uint32_t starting_position = high_index_aux + low_index_aux;

  /* TODO: Add the condition when we need to read 2 sectors to get the data (12 bytes against 4096) */
  /* Read data in memory (ID + 4 bytes based on initial position) */
  if(W25Q80DV_ReadBytes(starting_position, (uint8_t*)aux, EXTFLASH_BYTES_TO_READ_DATA) == W25Q80DV_OK)
  {
    *x_mag = (int16_t)((aux[0] << 8) | aux[1]);
	*y_mag = (int16_t)((aux[2] << 8) | aux[3]);
	*z_mag = (int16_t)((aux[4] << 8) | aux[5]);
	*temp = (int16_t)((aux[6] << 8) | aux[7]);
	retval = EXTFLASH_OK;
  }

  return retval;
}

/**
  * @brief Writes data to external FLASH memory
  * @param id_value: ID to look for in FLASH memory
  * @param x_mag: magnetometer X value to write
  * @param y_mag: magnetometer Y value to write
  * @param z_mag: magnetometer Z value to write
  * @param temp: temperature value to write
  * @return EXTFLASH Status
  */
EXTFLASH_StatusTypeDef EXTFLASH_WriteData(uint32_t id_value, int16_t x_mag, int16_t y_mag, int16_t z_mag, int16_t temp)
{
  EXTFLASH_StatusTypeDef retval = EXTFLASH_ERROR;
  uint8_t sector_data[W25Q80DV_SECTOR_SIZE];
  uint32_t index_aux, write_retrials;

  /* TODO: Add the condition when we need to write 2 sectors (12 bytes against 4096) */
  index_aux = (id_value & (~(W25Q80DV_SECTOR_SIZE-1))) * W25Q80DV_SECTOR_SIZE;
  /* First, read data of target sector (mask to read the beginning of that sector) */
  if(W25Q80DV_ReadSector(index_aux, sector_data) == W25Q80DV_OK)
  {
	  /* Then, erase sector */
	  index_aux = (id_value & (W25Q80DV_SECTOR_SIZE-1)) * EXTFLASH_BYTES_STORED_PER_FIELD;
	  if(W25Q80DV_EraseSector((id_value & (~(W25Q80DV_SECTOR_SIZE-1))) * W25Q80DV_SECTOR_SIZE) == W25Q80DV_OK)
	  {
		  /* Finally, write the new values and write the sector in memory */
		  sector_data[index_aux] = ((id_value >> 24) & 0xFF);
		  sector_data[index_aux + 1] = ((id_value >> 16) & 0xFF);
		  sector_data[index_aux + 2] = ((id_value >> 8) & 0xFF);
		  sector_data[index_aux + 3] = (id_value & 0xFF);
		  sector_data[index_aux + 4] = (x_mag >> 8);
		  sector_data[index_aux + 5] = (x_mag & 0xFF);
		  sector_data[index_aux + 6] = (y_mag >> 8);
		  sector_data[index_aux + 7] = (y_mag & 0xFF);
		  sector_data[index_aux + 8] = (z_mag >> 8);
		  sector_data[index_aux + 9] = (z_mag & 0xFF);
		  sector_data[index_aux + 10] = (temp >> 8);
		  sector_data[(id_value & (W25Q80DV_SECTOR_SIZE-1)) * EXTFLASH_BYTES_STORED_PER_FIELD + 11] = (temp & 0xFF);

		  /* At this point, if data is not written in memory, all the information in that sector
		   * would be lost, so the loop tries to minimize this to happen */
		  for(write_retrials = 0; write_retrials < EXTFLASH_MAX_WRITE_RETRIALS; write_retrials++)
		  {
			  if(W25Q80DV_WriteSector(((id_value & (~(W25Q80DV_SECTOR_SIZE-1))) * W25Q80DV_SECTOR_SIZE), (uint8_t*)sector_data) == W25Q80DV_OK)
			  {
				  retval = EXTFLASH_OK;
				  break;
			  }
		  }
	  }
  }
  return retval;
}
