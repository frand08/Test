/**
  ******************************************************************************
  * @file w25q80dv_conf.h
  * @author fdominguez
  * @date 01/13/2020
  * @version 1.0.0
  ******************************************************************************
  */

#ifndef W25Q80DV_CONF_H_
#define W25Q80DV_CONF_H_

#include "w25q80dv.h"
#include "main.h"

/* Enable/Disable this option if you want to use SPI via DMA */
#define W25Q80DV_USE_DMA

#define W25Q80DV_CS_ON									GPIO_PIN_RESET
#define W25Q80DV_CS_OFF									GPIO_PIN_SET

/* Functions to be implemented by user */
void W25Q80DV_ChipSelect(uint32_t on_off);
void W25Q80DV_Delay(uint32_t ms);
W25Q80DV_StatusTypeDef W25Q80DV_TxRx(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout);
W25Q80DV_StatusTypeDef W25Q80DV_Tx(uint8_t *pData, uint16_t Size, uint32_t Timeout);
W25Q80DV_StatusTypeDef W25Q80DV_Rx(uint8_t *pData, uint16_t Size, uint32_t Timeout);
W25Q80DV_StatusTypeDef W25Q80DV_TxRx_DMA(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);
W25Q80DV_StatusTypeDef W25Q80DV_Tx_DMA(uint8_t *pData, uint16_t Size);
W25Q80DV_StatusTypeDef W25Q80DV_Rx_DMA(uint8_t *pData, uint16_t Size);
W25Q80DV_StatusTypeDef W25Q80DV_Tx_DMA_WaitToFinish(uint32_t timeout);
W25Q80DV_StatusTypeDef W25Q80DV_Rx_DMA_WaitToFinish(uint32_t timeout);
#endif /* W25Q80DV_CONF_H_ */
