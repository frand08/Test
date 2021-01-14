/*
 * lis3mdl.c
 *
 *  Created on: Jan 13, 2021
 *      Author: fdominguez
 */

#include "lis3mdl.h"


/**
  * @brief Initializes the magnetometer
  * @retval LIS3MDL Status
  */
LIS3MDL_StatusTypeDef LIS3MDL_Init(void)
{
	uint8_t tx_data[2], rx_data;
	int retrials;

	LIS3MDL_CtrlReg1TypeDef ctrl_reg1 = {0};
	LIS3MDL_CtrlReg2TypeDef ctrl_reg2 = {0};
	LIS3MDL_CtrlReg3TypeDef ctrl_reg3 = {0};
	LIS3MDL_CtrlReg4TypeDef ctrl_reg4 = {0};
	LIS3MDL_CtrlReg5TypeDef ctrl_reg5 = {0};

	LIS3MDL_StatusTypeDef retval = LIS3MDL_ERROR;


	/* Enable CS pin and wait until stabilizes */
	LIS3MDL_SPI_CS(LIS3MDL_CS_ON);
	LIS3MDL_Delay(1);

	/* Try to init up to LIS3MDL_INIT_RETRIALS times */
	for(retrials = 1; retrials <= LIS3MDL_INIT_RETRIALS; retrials++)
	{
		/* In order to read, R/W = 1 */
		tx_data[0] = (LIS3MDL_WHO_AM_I | 0x80);
		LIS3MDL_TxRx(tx_data, &rx_data, 1, 100);

		/* If who am i read OK, continue, otherwise error */
		if(rx_data == LIS3MDL_WHO_AM_I_RET)
		{
			/* Enable temperature reading */
			ctrl_reg1.TEMP_EN = 0b1;

			/* Set ultra high performance mode */
			ctrl_reg1.OM = 0b11;

			/* Output data rate 10 Hz */
			ctrl_reg1.DO = 0b100;

			/* Disable fast ODR */
			ctrl_reg1.FAST_ODR = 0b0;

			/* Self test disabled */
			ctrl_reg1.ST = 0;

			tx_data[0] = LIS3MDL_CTRL_REG1;
			tx_data[1] = ctrl_reg1.chars;
			LIS3MDL_Tx(tx_data, 2, 100);

			/* Set +-12 gauss resolution */
			ctrl_reg2.FS = 0b10;

			/* Do not reboot memory content */
			ctrl_reg2.REBOOT = 0b0;

			/* Do not soft reset */
			ctrl_reg2.SOFT_RST = 0b0;

			tx_data[0] = LIS3MDL_CTRL_REG2;
			tx_data[1] = ctrl_reg2.chars;
			LIS3MDL_Tx(tx_data, 2, 100);

			/* Do not enable low power */
			ctrl_reg3.LP = 0b0;

			/* 4-wire SPI */
			ctrl_reg3.SIM = 0b0;

			/* Continuous conversion mode */
			ctrl_reg3.MD = 0b00;

			tx_data[0] = LIS3MDL_CTRL_REG3;
			tx_data[1] = ctrl_reg3.chars;
			LIS3MDL_Tx(tx_data, 2, 100);

			/* Set Z-axis to ultra high performance mode */
			ctrl_reg4.OMZ = 0b11;

			/* Data LSB at lower address */
			ctrl_reg4.BLE = 0b0;

			tx_data[0] = LIS3MDL_CTRL_REG4;
			tx_data[1] = ctrl_reg4.chars;
			LIS3MDL_Tx(tx_data, 2, 100);

			/* Disable fast read */
			ctrl_reg5.FAST_READ = 0b0;

			/* Update data periodically */
			ctrl_reg5.BDU = 0b0;

			tx_data[0] = LIS3MDL_CTRL_REG5;
			tx_data[1] = ctrl_reg5.chars;
			LIS3MDL_Tx(tx_data, 2, 100);

			retval = LIS3MDL_OK;
			break;
		}
		else
		{
			/* TODO: Do something when WHO_AM_I read error */
			LIS3MDL_Delay(10);
		}
	}

	/* Disable CS pin and wait until stabilizes */
	LIS3MDL_SPI_CS(LIS3MDL_CS_OFF);
	LIS3MDL_Delay(1);

	return retval;
}

/**
  * @brief Read data values
  * @param data: LIS3MDL Data
  */
void LIS3MDL_ReadValues(LIS3MDL_DataTypeDef *data)
{
	uint8_t tx_data, rx_data[8];

	/* Enable CS pin and wait until stabilizes */
	LIS3MDL_SPI_CS(LIS3MDL_CS_ON);
	LIS3MDL_Delay(1);

	/* Burst read all values (set R/W = 1 and M/S = 1) */
	tx_data = (LIS3MDL_OUT_X_L | 0x80 | 0x40);
	LIS3MDL_Tx(&tx_data, 1, 100);

	LIS3MDL_Rx(rx_data, 8, 100);

	data->mag_x = (int16_t) ((rx_data[1] << 8) | rx_data[0]);
	data->mag_y = (int16_t) ((rx_data[3] << 8) | rx_data[2]);
	data->mag_z = (int16_t) ((rx_data[5] << 8) | rx_data[4]);
	data->temp = (int16_t) ((rx_data[7] << 8) | rx_data[6]);

	/* Disable CS pin and wait until stabilizes */
	LIS3MDL_SPI_CS(LIS3MDL_CS_OFF);
	LIS3MDL_Delay(1);
}
