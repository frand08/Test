/**
  ******************************************************************************
  * @file lis3mdl_conf.c
  * @author fdominguez
  * @brief User configuration for LIS3MDL module driver
  * @date 01/13/2020
  * @version 1.0.0
  ******************************************************************************
  */

#include "w25q80dv_conf.h"
#include "cmsis_os.h"
#include "stm32f1xx_hal.h"

extern SPI_HandleTypeDef hspi1;
extern osMessageQId SPITxQueueHandle;
extern osMessageQId SPIRxQueueHandle;

/**
  * @brief ON/OFF magnetometer chip select pin
  * @param on_off: pin state selected
  */
void W25Q80DV_ChipSelect(uint32_t on_off)
{
	HAL_GPIO_WritePin(CS_MAG_GPIO_Port, CS_MAG_Pin, on_off);
}

/**
  * @brief Delay in milliseconds
  * @param ms: milliseconds to delay (should be a non-blocking function)
  */
void W25Q80DV_Delay(uint32_t ms)
{
	osDelay(ms);
}

/**
  * @brief Transmit and then receives a number of bytes
  * @param tx_data: Vector to transmit
  * @param rx_data: Vector where the data read is stored
  * @param size: Number of bytes to transmit/receive
  * @param  Timeout Timeout duration
  * @retval W25Q80DV status
  */
W25Q80DV_StatusTypeDef W25Q80DV_TxRx(uint8_t *tx_data, uint8_t *rx_data, uint16_t size, uint32_t timeout)
{
	W25Q80DV_StatusTypeDef retval = W25Q80DV_ERROR;

	if(HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, size, timeout) == HAL_OK)
		retval = W25Q80DV_OK;

	return retval;
}

/**
  * @brief Transmits a number of bytes
  * @param tx_data: Vector to transmit
  * @param size: Number of bytes to transmit
  * @param  Timeout Timeout duration
  * @retval W25Q80DV status
  */
W25Q80DV_StatusTypeDef W25Q80DV_Tx(uint8_t *tx_data, uint16_t size, uint32_t timeout)
{
	W25Q80DV_StatusTypeDef retval = W25Q80DV_ERROR;

	if(HAL_SPI_Transmit(&hspi1, tx_data, size, timeout) == HAL_OK)
		retval = W25Q80DV_OK;

	return retval;
}

/**
  * @brief Receives a number of bytes
  * @param rx_data: Vector where the data read is stored
  * @param size: Number of bytes to receive
  * @param  Timeout Timeout duration
  * @retval W25Q80DV status
  */
W25Q80DV_StatusTypeDef W25Q80DV_Rx(uint8_t *rx_data, uint16_t size, uint32_t timeout)
{
	W25Q80DV_StatusTypeDef retval = W25Q80DV_ERROR;
	if(HAL_SPI_Receive(&hspi1, rx_data, size, timeout) == HAL_OK)
		retval = W25Q80DV_OK;

	return retval;
}

/**
  * @brief Transmit and then receives a number of bytes using DMA
  * @param tx_data: Vector to transmit
  * @param rx_data: Vector where the data read is stored
  * @param size: Number of bytes to transmit/receive
  * @retval W25Q80DV status
  */
W25Q80DV_StatusTypeDef W25Q80DV_TxRx_DMA(uint8_t *tx_data, uint8_t *rx_data, uint16_t size)
{
	W25Q80DV_StatusTypeDef retval = W25Q80DV_ERROR;

	if(HAL_SPI_TransmitReceive_DMA(&hspi1, tx_data, rx_data, size) == HAL_OK)
		retval = W25Q80DV_OK;

	return retval;
}

/**
  * @brief Transmits a number of bytes using DMA
  * @param tx_data: Vector to transmit
  * @param size: Number of bytes to transmit
  * @retval W25Q80DV status
  */
W25Q80DV_StatusTypeDef W25Q80DV_Tx_DMA(uint8_t *tx_data, uint16_t size)
{
	W25Q80DV_StatusTypeDef retval = W25Q80DV_ERROR;

	if(HAL_SPI_Transmit_DMA(&hspi1, tx_data, size) == HAL_OK)
		retval = W25Q80DV_OK;

	return retval;
}

/**
  * @brief Receives a number of bytes using DMA
  * @param rx_data: Vector where the data read is stored
  * @param size: Number of bytes to receive
  * @retval W25Q80DV status
  */
W25Q80DV_StatusTypeDef W25Q80DV_Rx_DMA(uint8_t *rx_data, uint16_t size)
{
	W25Q80DV_StatusTypeDef retval = W25Q80DV_ERROR;

	if(HAL_SPI_Receive_DMA(&hspi1, rx_data, size) == HAL_OK)
		retval = W25Q80DV_OK;

	return retval;
}

/**
  * @brief Waits for last SPI transmit operation to be completed
  * @param  Timeout Timeout duration
  * @retval W25Q80DV status
  */
W25Q80DV_StatusTypeDef W25Q80DV_Tx_DMA_WaitToFinish(uint32_t timeout)
{
	osEvent event;
	W25Q80DV_StatusTypeDef retval = W25Q80DV_ERROR;

	event = osMessageGet(SPITxQueueHandle, timeout);
	if(event.status == osEventMessage)
		retval = W25Q80DV_OK;

	return retval;
}

/**
  * @brief Waits for last SPI receive operation to be completed
  * @param  Timeout Timeout duration
  * @retval W25Q80DV status
  */
W25Q80DV_StatusTypeDef W25Q80DV_Rx_DMA_WaitToFinish(uint32_t timeout)
{
	osEvent event;
	W25Q80DV_StatusTypeDef retval = W25Q80DV_ERROR;

	event = osMessageGet(SPIRxQueueHandle, timeout);
	if(event.status == osEventMessage)
		retval = W25Q80DV_OK;

	return retval;
}
