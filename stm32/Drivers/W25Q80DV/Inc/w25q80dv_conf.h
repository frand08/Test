/*
 * w25q80dv_conf.h
 *
 *  Created on: Jan 13, 2021
 *      Author: fdominguez
 */

#ifndef W25Q80DV_CONF_H_
#define W25Q80DV_CONF_H_

#include "cmsis_os.h"
#include "stm32f1xx_hal.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

#define W25Q80DV_SPI									hspi1

#define W25Q80DV_CS_ON									GPIO_PIN_RESET
#define W25Q80DV_CS_OFF									GPIO_PIN_SET
#define W25Q80DV_SPI_CS(on_off)							HAL_GPIO_WritePin(CS_FLASH_GPIO_Port, CS_FLASH_Pin, on_off)

#define W25Q80DV_Delay(ms)								osDelay(ms)
#define W25Q80DV_TxRx(tx_data, rx_data, size, timeout)	HAL_SPI_TransmitReceive(&W25Q80DV_SPI, tx_data, rx_data, size, timeout)
#define W25Q80DV_Tx(tx_data, size, timeout)				HAL_SPI_Transmit(&W25Q80DV_SPI, tx_data, size, timeout)
#define W25Q80DV_Rx(rx_data, size, timeout)				HAL_SPI_Receive(&W25Q80DV_SPI, rx_data, size, timeout)

#endif /* W25Q80DV_CONF_H_ */
