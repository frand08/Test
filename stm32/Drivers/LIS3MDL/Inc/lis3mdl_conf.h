/*
 * lis3mdl_conf.h
 *
 *  Created on: Jan 13, 2021
 *      Author: fdominguez
 */

#ifndef LIS3MDL_CONF_H_
#define LIS3MDL_CONF_H_

#include "cmsis_os.h"
#include "stm32f1xx_hal.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

#define LIS3MDL_SPI									hspi1

#define LIS3MDL_CS_ON									GPIO_PIN_RESET
#define LIS3MDL_CS_OFF									GPIO_PIN_SET
#define LIS3MDL_SPI_CS(on_off)							HAL_GPIO_WritePin(CS_MAG_GPIO_Port, CS_MAG_Pin, on_off)

#define LIS3MDL_Delay(ms)								osDelay(ms)
#define LIS3MDL_TxRx(tx_data, rx_data, size, timeout)	HAL_SPI_TransmitReceive(&LIS3MDL_SPI, tx_data, rx_data, size, timeout)
#define LIS3MDL_Tx(tx_data, size, timeout)				HAL_SPI_Transmit(&LIS3MDL_SPI, tx_data, size, timeout)
#define LIS3MDL_Rx(rx_data, size, timeout)				HAL_SPI_Receive(&LIS3MDL_SPI, rx_data, size, timeout)

#endif /* LIS3MDL_CONF_H_ */
