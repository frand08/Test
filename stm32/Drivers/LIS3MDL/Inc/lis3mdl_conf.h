/**
  ******************************************************************************
  * @file lis3mdl_conf.h
  * @author fdominguez
  * @date 01/13/2020
  * @version 1.0.0
  ******************************************************************************
  */

#ifndef LIS3MDL_CONF_H_
#define LIS3MDL_CONF_H_

#include "lis3mdl.h"
#include "main.h"

/* Enable/Disable this option if you want to use SPI via DMA */
#define LIS3MDL_USE_DMA

#define LIS3MDL_CS_ON									GPIO_PIN_RESET
#define LIS3MDL_CS_OFF									GPIO_PIN_SET

/* Functions to be implemented by user */
void LIS3MDL_ChipSelect(uint32_t on_off);
void LIS3MDL_Delay(uint32_t ms);
LIS3MDL_StatusTypeDef LIS3MDL_TxRx(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout);
LIS3MDL_StatusTypeDef LIS3MDL_Tx(uint8_t *pData, uint16_t Size, uint32_t Timeout);
LIS3MDL_StatusTypeDef LIS3MDL_Rx(uint8_t *pData, uint16_t Size, uint32_t Timeout);
LIS3MDL_StatusTypeDef LIS3MDL_Tx_DMA(uint8_t *pData, uint16_t Size);
LIS3MDL_StatusTypeDef LIS3MDL_Rx_DMA(uint8_t *pData, uint16_t Size);
LIS3MDL_StatusTypeDef LIS3MDL_Tx_DMA_WaitToFinish(uint32_t timeout);
LIS3MDL_StatusTypeDef LIS3MDL_Rx_DMA_WaitToFinish(uint32_t timeout);
#endif /* LIS3MDL_CONF_H_ */
