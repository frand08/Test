/**
  ******************************************************************************
  * @file w25q80dv.c
  * @author fdominguez
  * @brief W25Q80DV serial SPI memory driver
  * @date 01/13/2020
  * @version 1.0.0
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================

    [..]
    *** How to adapt the library to your own project and use it ***
    ===========================================================
    [..]

    (#) Edit the functions defined in w25q80dv_conf.c to satisfy what they
        require, using the specific functions you have.
    (#) Select if you want to use DMA (non-blocking) or not (blocking).
    (#) Include the w25q80dv.h where you want to use the library.
    (#) Initialize the library with W25Q80DV_Init(), then if OK you can read
        or write data to flash memory
  @endverbatim
  ******************************************************************************
  */

#include "w25q80dv_conf.h"


/**
  * @brief Initializes the FLASH memory
  * @retval W25Q80DV Status
  */
W25Q80DV_StatusTypeDef W25Q80DV_Init(void)
{
	uint8_t tx_data, rx_data[3];
	uint32_t value, retrials;
	W25Q80DV_StatusTypeDef prevop_status = W25Q80DV_ERROR;

	/* Retry initialization W25Q80DV_RETRIAL times in case of failure */
	for(retrials = 0; retrials <= W25Q80DV_RETIRALS; retrials++)
	{
		/* Enable CS pin and wait until stabilizes */
		W25Q80DV_ChipSelect(W25Q80DV_CS_ON);
		W25Q80DV_Delay(10);

		/* Send ID */
		tx_data = W25Q80DV_ID;

#ifdef W25Q80DV_USE_DMA

		if(W25Q80DV_Tx_DMA(&tx_data, 1) == W25Q80DV_OK)
		{
			/* read values */
			if(W25Q80DV_Rx_DMA(rx_data, 3) == W25Q80DV_OK)
			{
				/* Wait up to one millisecond for the data to be received */
				prevop_status = W25Q80DV_Rx_DMA_WaitToFinish(1);
			}
		}

#else
		if(W25Q80DV_Tx(&tx_data, 1, 100) == W25Q80DV_OK)
		{
			/* read values */
			prevop_status = W25Q80DV_Rx(rx_data, 3, 100);
		}

#endif
		/* Composite read value */
		value = (rx_data[0] << 16) | (rx_data[1] << 8) | rx_data[2];

		/* Disable CS pin and wait until stabilizes */
		W25Q80DV_ChipSelect(W25Q80DV_CS_OFF);
		W25Q80DV_Delay(10);

		/* If wrong ID or previous operation error, reset the memory and try again
		 * up to W25Q80DV_RETIRALS times
		 */
		if(value != 0x4014 || prevop_status != W25Q80DV_OK)
		{
			/* Reset memory */
			W25Q80DV_Reset();
		}
		else
			break;
	}
	return prevop_status;
}

/**
  * @brief Enables write operation
  * @retval W25Q80DV Status
  */
W25Q80DV_StatusTypeDef W25Q80DV_WriteEnable(void)
{
	uint8_t tx_data;
	W25Q80DV_StatusTypeDef retval = W25Q80DV_ERROR;

	/* Enable CS pin and wait until stabilizes */
	W25Q80DV_ChipSelect(W25Q80DV_CS_ON);
	W25Q80DV_Delay(10);

	/* Enable write */
	tx_data = W25Q80DV_WRITE_ENABLE;

#ifdef W25Q80DV_USE_DMA
	if(W25Q80DV_Tx_DMA(&tx_data, 1) == W25Q80DV_OK)
	{
		/* Wait up to one millisecond for the data to be transmitted */
		retval = W25Q80DV_Tx_DMA_WaitToFinish(1);
	}

#else
	retval = W25Q80DV_Tx(&tx_data, 1, 100);
#endif
	/* Disable CS pin and wait until stabilizes */
	W25Q80DV_ChipSelect(W25Q80DV_CS_OFF);
	W25Q80DV_Delay(10);

	return retval;
}

/**
  * @brief Disables write operation
  * @retval W25Q80DV Status
  */
W25Q80DV_StatusTypeDef W25Q80DV_WriteDisable(void)
{
	uint8_t tx_data;
	W25Q80DV_StatusTypeDef retval = W25Q80DV_ERROR;

	/* Enable CS pin and wait until stabilizes */
	W25Q80DV_ChipSelect(W25Q80DV_CS_ON);
	W25Q80DV_Delay(10);

	/* Disable write */
	tx_data = W25Q80DV_WRITE_DISABLE;

#ifdef W25Q80DV_USE_DMA
	if(W25Q80DV_Tx_DMA(&tx_data, 1) == W25Q80DV_OK)
	{
		/* Wait up to one millisecond for the data to be transmitted */
		retval = W25Q80DV_Tx_DMA_WaitToFinish(1);
	}

#else
	retval = W25Q80DV_Tx(&tx_data, 1, 100);
#endif

	/* Disable CS pin and wait until stabilizes */
	W25Q80DV_ChipSelect(W25Q80DV_CS_OFF);
	W25Q80DV_Delay(10);

	return retval;
}

/**
  * @brief Reset FLASH memory
  * @retval W25Q80DV Status
  */
W25Q80DV_StatusTypeDef W25Q80DV_Reset(void)
{
	W25Q80DV_StatusTypeDef prevop_status = W25Q80DV_ERROR;
	uint8_t tx_data;
	W25Q80DV_StatusRegTypeDef status;
	/* Enable CS pin and wait until stabilizes */
	W25Q80DV_ChipSelect(W25Q80DV_CS_ON);
	W25Q80DV_Delay(10);

	/* Check the BUSY bit and the SUS bit in Status register
	 * before the reset command sequence to avoid data
	 * corruption
	 */
	prevop_status = W25Q80DV_ReadStatusRegister((uint8_t*)&status);

	if(prevop_status == W25Q80DV_OK && status.SUS == 0 && status.BUSY == 0)
	{
		/* Enable reset */
		tx_data = W25Q80DV_ENABLE_RESET;

#ifdef W25Q80DV_USE_DMA
		if(W25Q80DV_Tx_DMA(&tx_data, 1) == W25Q80DV_OK)
		{
			/* Wait up to one millisecond for the data to be transmitted */
			prevop_status = W25Q80DV_Tx_DMA_WaitToFinish(1);
		}
		else
			prevop_status = W25Q80DV_ERROR;

#else
		prevop_status = W25Q80DV_Tx(&tx_data, 1, 100);
#endif

		/* Disable CS pin and wait until stabilizes */
		W25Q80DV_ChipSelect(W25Q80DV_CS_OFF);
		W25Q80DV_Delay(10);

		if(prevop_status == W25Q80DV_OK)
		{
			/* Enable CS pin and wait until stabilizes */
			W25Q80DV_ChipSelect(W25Q80DV_CS_ON);
			W25Q80DV_Delay(10);


			/* reset memory */
			tx_data = W25Q80DV_RESET;

#ifdef W25Q80DV_USE_DMA
			if(W25Q80DV_Tx_DMA(&tx_data, 1) == W25Q80DV_OK)
			{
				/* Wait up to one millisecond for the data to be transmitted */
				prevop_status = W25Q80DV_Tx_DMA_WaitToFinish(1);
			}
			else
				prevop_status = W25Q80DV_ERROR;

#else
			prevop_status = W25Q80DV_Tx(&tx_data, 1, 100);
#endif

			/* Disable CS pin and wait until stabilizes */
			W25Q80DV_ChipSelect(W25Q80DV_CS_OFF);
			W25Q80DV_Delay(10);
		}
	}

	return prevop_status;
}

/**
  * @brief Read some bytes based on initial position
  * @param init_pos: Position where the read action begins
  * @param count: Number of bytes to read
  * @retval W25Q80DV Status
  */
W25Q80DV_StatusTypeDef W25Q80DV_ReadBytes(uint32_t init_pos, uint8_t* data, uint32_t count)
{
	uint8_t aux_data[4];
	W25Q80DV_StatusTypeDef retval = W25Q80DV_ERROR;

	/* Enable CS pin and wait until stabilizes */
	W25Q80DV_ChipSelect(W25Q80DV_CS_ON);
	W25Q80DV_Delay(10);

	/* Send read command with sector initial position */
	aux_data[0] = W25Q80DV_READ;
	aux_data[1] = (init_pos >> 16) & 0xFF;
	aux_data[2] = (init_pos >> 8) & 0xFF;
	aux_data[3] = (init_pos) & 0xFF;

#ifdef W25Q80DV_USE_DMA
	if(W25Q80DV_Tx_DMA(aux_data, 4) == W25Q80DV_OK)
	{
		if(W25Q80DV_Rx_DMA(data, count) == W25Q80DV_OK)
		{
			/* Wait up to 2 milliseconds for the data to be received */
			retval = W25Q80DV_Rx_DMA_WaitToFinish(2);
		}
	}

#else
	if(W25Q80DV_Tx(aux_data, 4, 100) == W25Q80DV_OK)
		retval = W25Q80DV_Rx(data, count, 100);
#endif

	/* Disable CS pin and wait until stabilizes */
	W25Q80DV_ChipSelect(W25Q80DV_CS_OFF);
	W25Q80DV_Delay(10);

	W25Q80DV_WriteDisable();

	return retval;
}

/**
  * @brief Reads a complete sector starting in init_pos (24 bits)
  *	@param init_pos: Position where the sector begins
  *	@param received_data: Data read (W25Q80DV_SECTOR_SIZE)
  * @retval W25Q80DV Status
  */
W25Q80DV_StatusTypeDef W25Q80DV_ReadSector(uint32_t init_pos, uint8_t* received_data)
{
	uint8_t tx_data[4];
	W25Q80DV_StatusTypeDef retval = W25Q80DV_ERROR;

	/* Enable CS pin and wait until stabilizes */
	W25Q80DV_ChipSelect(W25Q80DV_CS_ON);
	W25Q80DV_Delay(10);


	/* Send read command with sector initial position */
	tx_data[0] = W25Q80DV_READ;
	tx_data[1] = (init_pos >> 16) & 0xFF;
	tx_data[2] = (init_pos >> 8) & 0xFF;
	tx_data[3] = (init_pos) & 0xFF;

#ifdef W25Q80DV_USE_DMA
	if(W25Q80DV_Tx_DMA(tx_data, 4) == W25Q80DV_OK)
	{
		/* Read the sector secuentially */
		if(W25Q80DV_Rx_DMA(received_data, W25Q80DV_SECTOR_SIZE) == W25Q80DV_OK)
		{
			/* Wait up to 5 milliseconds for the data to be received */
			retval = W25Q80DV_Rx_DMA_WaitToFinish(5);
		}
	}

#else
	if(W25Q80DV_Tx(tx_data, 4, 100) == W25Q80DV_OK)
	{
		/* Read the sector secuentially */
		retval = W25Q80DV_Rx(received_data, W25Q80DV_SECTOR_SIZE, 1000);
	}
#endif

	/* Disable CS pin and wait until stabilizes */
	W25Q80DV_ChipSelect(W25Q80DV_CS_OFF);
	W25Q80DV_Delay(10);

	return retval;
}


/**
  * @brief Reads status register. It is recommended to use
  * the W25Q80DV_StatusRegTypeDef to read the value
  * @param data: Data read (2 bytes)
  * @retval W25Q80DV Status
  */
W25Q80DV_StatusTypeDef W25Q80DV_ReadStatusRegister(uint8_t* data)
{
	uint8_t tx_data;
	W25Q80DV_StatusTypeDef prevop_status = W25Q80DV_ERROR;

	/* Enable CS pin and wait until stabilizes */
	W25Q80DV_ChipSelect(W25Q80DV_CS_ON);
	W25Q80DV_Delay(10);

	/* Send Status register 1 and read it */
	tx_data = W25Q80DV_STATUS_REG_1;

#ifdef W25Q80DV_USE_DMA
	if(W25Q80DV_TxRx_DMA(&tx_data, &data[0], 1) == W25Q80DV_OK)
	{
		/* Wait up to one millisecond for the data to be received */
		prevop_status = W25Q80DV_Rx_DMA_WaitToFinish(1);
	}

#else
	prevop_status = W25Q80DV_TxRx(&tx_data, &data[0], 1, 100);

#endif

	if(prevop_status == W25Q80DV_OK)
	{
		/* Send Status register 2 and read it */
		tx_data = W25Q80DV_STATUS_REG_2;

#ifdef W25Q80DV_USE_DMA
		if(W25Q80DV_TxRx_DMA(&tx_data, &data[1], 1) == W25Q80DV_OK)
		{
			/* Wait up to one millisecond for the data to be received */
			prevop_status = W25Q80DV_Rx_DMA_WaitToFinish(1);
		}
#else
		prevop_status = W25Q80DV_TxRx(&tx_data, &data[1], 1, 100);

#endif
    }

	/* Disable CS pin and wait until stabilizes */
	W25Q80DV_ChipSelect(W25Q80DV_CS_OFF);
	W25Q80DV_Delay(10);

	return prevop_status;
}

/**
  * @brief Erases a complete sector starting in init_pos (24 bits)
  * @param init_pos: Position where the sector begins
  * @retval W25Q80DV Status
  */
W25Q80DV_StatusTypeDef W25Q80DV_EraseSector(uint32_t init_pos)
{
	uint8_t tx_data[4];
	W25Q80DV_StatusTypeDef retval = W25Q80DV_ERROR;

	/* Enable CS pin and wait until stabilizes */
	W25Q80DV_ChipSelect(W25Q80DV_CS_ON);
	W25Q80DV_Delay(10);

	/* Send read command with sector position */
	tx_data[0] = W25Q80DV_ERASE_SECTOR;
	tx_data[1] = (init_pos >> 16) & 0xFF;
	tx_data[2] = (init_pos >> 8) & 0xFF;
	tx_data[3] = (init_pos) & 0xFF;

#ifdef W25Q80DV_USE_DMA
	if(W25Q80DV_Tx_DMA(tx_data, 4) == W25Q80DV_OK)
	{
		/* Wait up to one millisecond for the data to be received */
		retval = W25Q80DV_Rx_DMA_WaitToFinish(1);
	}

#else
	retval = W25Q80DV_Tx(tx_data, 4, 100);

#endif

	/* Disable CS pin and wait until stabilizes */
	W25Q80DV_ChipSelect(W25Q80DV_CS_OFF);
	W25Q80DV_Delay(10);

	return retval;
}

/**
  * @brief Writes a complete sector starting in init_pos (24 bits)
  * @param init_pos: Position where the sector begins
  * @param data: Data to be written
  * @retval W25Q80DV Status
  */
W25Q80DV_StatusTypeDef W25Q80DV_WriteSector(uint32_t init_pos, uint8_t* data)
{
	uint8_t aux_data[4];
	W25Q80DV_StatusTypeDef retval = W25Q80DV_ERROR;

	W25Q80DV_WriteEnable();

	/* Enable CS pin and wait until stabilizes */
	W25Q80DV_ChipSelect(W25Q80DV_CS_ON);
	W25Q80DV_Delay(10);

	/* Send read command with sector initial position */
	aux_data[0] = W25Q80DV_PAGE_PROGRAM;
	aux_data[1] = (init_pos >> 16) & 0xFF;
	aux_data[2] = (init_pos >> 8) & 0xFF;
	aux_data[3] = (init_pos) & 0xFF;

#ifdef W25Q80DV_USE_DMA

	if(W25Q80DV_Tx_DMA(aux_data, 4) == W25Q80DV_OK)
	{
		/* Wait up to one millisecond for the data to be transmitted */
		if(W25Q80DV_Tx_DMA_WaitToFinish(1) == W25Q80DV_OK)
		{
			/* Then, send data */
			if(W25Q80DV_Tx_DMA(data, W25Q80DV_SECTOR_SIZE) == W25Q80DV_OK)
			{
				/* Wait up to 5 milliseconds for the data to be transmitted */
				retval = W25Q80DV_Tx_DMA_WaitToFinish(5);
			}
		}
	}

#else
	if(W25Q80DV_Tx(aux_data, 4, 100) == W25Q80DV_OK)
		retval = W25Q80DV_Tx(data, W25Q80DV_SECTOR_SIZE, 1000);

#endif

	/* Disable CS pin and wait until stabilizes */
	W25Q80DV_ChipSelect(W25Q80DV_CS_OFF);
	W25Q80DV_Delay(10);

	W25Q80DV_WriteDisable();

	return retval;
}
