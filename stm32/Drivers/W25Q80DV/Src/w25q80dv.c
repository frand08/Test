/*
 * w25q80dv.c
 *
 *  Created on: Jan 13, 2021
 *      Author: fdominguez
 */

#include "w25q80dv.h"


/**
  * @brief Initializes the FLASH memory
  * @retval W25Q80DV Status
  */
W25Q80DV_StatusTypeDef W25Q80DV_Init(void)
{
	uint8_t tx_data, rx_data[3];
	uint32_t value, retrials;
	W25Q80DV_StatusTypeDef retval = W25Q80DV_ERROR;

	for(retrials = 0; retrials <= W25Q80DV_RETIRALS; retrials++)
	{
		/* Enable CS pin and wait until stabilizes */
		W25Q80DV_SPI_CS(W25Q80DV_CS_ON);
		W25Q80DV_Delay(100);

		/* Send ID */
		tx_data = W25Q80DV_ID;
		W25Q80DV_Tx(&tx_data, 1, 100);

		/* read values */
		W25Q80DV_Rx(rx_data, 3, 100);

		/* Composite read value */
		value = (rx_data[0] << 16) | (rx_data[1] << 8) | rx_data[2];

		/* Disable CS pin and wait until stabilizes */
		W25Q80DV_SPI_CS(W25Q80DV_CS_OFF);
		W25Q80DV_Delay(100);

		/* If wrong ID, reset the memory and try again
		 * up to W25Q80DV_RETIRALS times
		 */
		if(value != 0x4014)
		{
			/* Reset memory */
			W25Q80DV_Reset();
		}
		else
		{

			retval = W25Q80DV_OK;

			break;
		}
	}
	return retval;
}

/**
  * @brief Enables write operation
  */
void W25Q80DV_WriteEnable(void)
{
	uint8_t tx_data;

	/* Enable CS pin and wait until stabilizes */
	W25Q80DV_SPI_CS(W25Q80DV_CS_ON);
	W25Q80DV_Delay(100);

	/* Enable write */
	tx_data = W25Q80DV_WRITE_ENABLE;
	W25Q80DV_Tx(&tx_data, 1, 100);

	/* Disable CS pin and wait until stabilizes */
	W25Q80DV_SPI_CS(W25Q80DV_CS_OFF);
	W25Q80DV_Delay(100);
}

/**
  * @brief Disables write operation
  */
void W25Q80DV_WriteDisable(void)
{
	uint8_t tx_data;

	/* Enable CS pin and wait until stabilizes */
	W25Q80DV_SPI_CS(W25Q80DV_CS_ON);
	W25Q80DV_Delay(100);

	/* Disable write */
	tx_data = W25Q80DV_WRITE_DISABLE;
	W25Q80DV_Tx(&tx_data, 1, 100);

	/* Disable CS pin and wait until stabilizes */
	W25Q80DV_SPI_CS(W25Q80DV_CS_OFF);
	W25Q80DV_Delay(100);
}

/**
  * @brief Reset FLASH memory
  * @retval W25Q80DV Status
  */
W25Q80DV_StatusTypeDef W25Q80DV_Reset(void)
{
	W25Q80DV_StatusTypeDef retval = W25Q80DV_ERROR;
	uint8_t tx_data;
	W25Q80DV_StatusRegTypeDef status;
	/* Enable CS pin and wait until stabilizes */
	W25Q80DV_SPI_CS(W25Q80DV_CS_ON);
	W25Q80DV_Delay(100);

	/* Check the BUSY bit and the SUS bit in Status register
	 * before the reset command sequence to avoid data
	 * corruption
	 */
	W25Q80DV_ReadStatusRegister((uint8_t*)&status);

	if(status.SUS == 0 && status.BUSY == 0)
	{
		/* Enable reset */
		tx_data = W25Q80DV_ENABLE_RESET;
		W25Q80DV_Tx(&tx_data, 1, 100);

		/* Disable CS pin and wait until stabilizes */
		W25Q80DV_SPI_CS(W25Q80DV_CS_OFF);
		W25Q80DV_Delay(100);

		/* Enable CS pin and wait until stabilizes */
		W25Q80DV_SPI_CS(W25Q80DV_CS_ON);
		W25Q80DV_Delay(100);

		/* reset memory */
		tx_data = W25Q80DV_RESET;
		W25Q80DV_Tx(&tx_data, 1, 100);

		/* Disable CS pin and wait until stabilizes */
		W25Q80DV_SPI_CS(W25Q80DV_CS_OFF);
		W25Q80DV_Delay(100);
		retval = W25Q80DV_OK;
	}
	else
	{
		/* TODO: Check what should be done */
		W25Q80DV_Delay(100);
	}
	return retval;
}

/**
  * @brief Read some bytes based on initial position
  * @param init_pos: Position where the read action begins
  * @param count: Number of bytes to read
  */
void W25Q80DV_ReadBytes(uint32_t init_pos, uint8_t* data, uint32_t count)
{
	uint8_t aux_data[4];

	/* Enable CS pin and wait until stabilizes */
	W25Q80DV_SPI_CS(W25Q80DV_CS_ON);
	W25Q80DV_Delay(100);

	/* Send read command with sector initial position */
	aux_data[0] = W25Q80DV_READ;
	aux_data[1] = (init_pos >> 16) & 0xFF;
	aux_data[2] = (init_pos >> 8) & 0xFF;
	aux_data[3] = (init_pos) & 0xFF;
	W25Q80DV_Tx(aux_data, 4, 100);

	W25Q80DV_Rx(data, count, 1000);

	/* Disable CS pin and wait until stabilizes */
	W25Q80DV_SPI_CS(W25Q80DV_CS_OFF);
	W25Q80DV_Delay(100);

	W25Q80DV_WriteDisable();
}

/**
  * @brief Reads a complete sector starting in init_pos (24 bits)
  *	@param init_pos: Position where the sector begins
  *	@param received_data: Data read (W25Q80DV_SECTOR_SIZE)
  */
void W25Q80DV_ReadSector(uint32_t init_pos, uint8_t* received_data)
{
	uint8_t tx_data[4];

	/* Enable CS pin and wait until stabilizes */
	W25Q80DV_SPI_CS(W25Q80DV_CS_ON);
	W25Q80DV_Delay(100);


	/* Send read command with sector initial position */
	tx_data[0] = W25Q80DV_READ;
	tx_data[1] = (init_pos >> 16) & 0xFF;
	tx_data[2] = (init_pos >> 8) & 0xFF;
	tx_data[3] = (init_pos) & 0xFF;
	W25Q80DV_Tx(tx_data, 4, 100);

	/* Read the sector secuentially */
	W25Q80DV_Rx(received_data, W25Q80DV_SECTOR_SIZE, 1000);

	/* Disable CS pin and wait until stabilizes */
	W25Q80DV_SPI_CS(W25Q80DV_CS_OFF);
	W25Q80DV_Delay(100);
}


/**
  * @brief Reads status register. It is recommended to use
  * the W25Q80DV_StatusRegTypeDef to read the value
  * @param data: Data read (2 bytes)
  */
void W25Q80DV_ReadStatusRegister(uint8_t* data)
{
	uint8_t tx_data;

	/* Enable CS pin and wait until stabilizes */
	W25Q80DV_SPI_CS(W25Q80DV_CS_ON);
	W25Q80DV_Delay(100);

	/* Send Status register 1 and read it */
	tx_data = W25Q80DV_STATUS_REG_1;
	W25Q80DV_TxRx(&tx_data, &data[0], 1, 100);

	/* Send Status register 2 and read it */
	tx_data = W25Q80DV_STATUS_REG_2;
	W25Q80DV_TxRx(&tx_data, &data[1], 1, 100);

	/* Disable CS pin and wait until stabilizes */
	W25Q80DV_SPI_CS(W25Q80DV_CS_OFF);
	W25Q80DV_Delay(100);
}

/**
  * @brief Erases a complete sector starting in init_pos (24 bits)
  * @param init_pos: Position where the sector begins
  */
void W25Q80DV_EraseSector(uint32_t init_pos)
{
	uint8_t tx_data[4];

	/* Enable CS pin and wait until stabilizes */
	W25Q80DV_SPI_CS(W25Q80DV_CS_ON);
	W25Q80DV_Delay(100);

	/* Send read command with sector position */
	tx_data[0] = W25Q80DV_ERASE_SECTOR;
	tx_data[1] = (init_pos >> 16) & 0xFF;
	tx_data[2] = (init_pos >> 8) & 0xFF;
	tx_data[3] = (init_pos) & 0xFF;
	W25Q80DV_Tx(tx_data, 4, 100);

	/* Disable CS pin and wait until stabilizes */
	W25Q80DV_SPI_CS(W25Q80DV_CS_OFF);
	W25Q80DV_Delay(100);
}

/**
  * @brief Writes a complete sector starting in init_pos (24 bits)
  * @param init_pos: Position where the sector begins
  * @param data: Data to be written
  */
void W25Q80DV_WriteSector(uint32_t init_pos, uint8_t* data)
{
	uint8_t aux_data[4];

	W25Q80DV_WriteEnable();

	/* Enable CS pin and wait until stabilizes */
	W25Q80DV_SPI_CS(W25Q80DV_CS_ON);
	W25Q80DV_Delay(100);

	/* Send read command with sector initial position */
	aux_data[0] = W25Q80DV_PAGE_PROGRAM;
	aux_data[1] = (init_pos >> 16) & 0xFF;
	aux_data[2] = (init_pos >> 8) & 0xFF;
	aux_data[3] = (init_pos) & 0xFF;
	W25Q80DV_Tx(aux_data, 4, 100);

	W25Q80DV_Tx(data, W25Q80DV_SECTOR_SIZE, 1000);

	/* Disable CS pin and wait until stabilizes */
	W25Q80DV_SPI_CS(W25Q80DV_CS_OFF);
	W25Q80DV_Delay(100);

	W25Q80DV_WriteDisable();
}
