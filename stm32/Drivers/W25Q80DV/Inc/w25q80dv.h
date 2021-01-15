/**
  ******************************************************************************
  * @file w25q80dv.h
  * @author fdominguez
  * @date 01/13/2020
  * @version 1.0.0
  ******************************************************************************
  */

#ifndef W25Q80DV_H_
#define W25Q80DV_H_

#include <stdint.h>

/* Number of retrials when initializing the memory */
#define W25Q80DV_RETIRALS		4

#define W25Q80DV_ID				0x9F
#define W25Q80DV_WRITE_ENABLE	0x06
#define W25Q80DV_WRITE_DISABLE	0x04
#define W25Q80DV_ENABLE_RESET	0x66
#define W25Q80DV_RESET			0x99
#define W25Q80DV_READ			0x03
#define W25Q80DV_ERASE_SECTOR	0x20
#define W25Q80DV_PAGE_PROGRAM	0x02
#define W25Q80DV_STATUS_REG_1	0x05
#define W25Q80DV_STATUS_REG_2	0x35

/* Bytes per sector */
#define W25Q80DV_SECTOR_SIZE	4096
/* Based on sector size */
#define W25Q80DV_BLOCK_SIZE		16
/* Based on block size */
#define W25Q80DV_PAGE_SIZE		16


typedef enum
{
  W25Q80DV_ERROR = -1,
  W25Q80DV_OK    = 0
} W25Q80DV_StatusTypeDef;

typedef struct
{
   uint8_t BUSY: 1;
  uint8_t WEL: 1;
  uint8_t BP0: 1;
  uint8_t BP1: 1;
  uint8_t BP2: 1;
  uint8_t TB: 1;
  uint8_t SEC: 1;
  uint8_t SRP0: 1;
  uint8_t SRP1: 1;
  uint8_t QE: 1;
  uint8_t resv: 1;
  uint8_t LB1: 1;
  uint8_t LB2: 1;
  uint8_t LB3: 1;
  uint8_t CMP: 1;
  uint8_t SUS: 1;
} W25Q80DV_StatusRegTypeDef;


W25Q80DV_StatusTypeDef W25Q80DV_Reset(void);
W25Q80DV_StatusTypeDef W25Q80DV_WriteEnable(void);
W25Q80DV_StatusTypeDef W25Q80DV_Init(void);
W25Q80DV_StatusTypeDef W25Q80DV_WriteDisable(void);
W25Q80DV_StatusTypeDef W25Q80DV_ReadBytes(uint32_t init_pos, uint8_t* data, uint32_t count);
W25Q80DV_StatusTypeDef W25Q80DV_ReadSector(uint32_t init_pos, uint8_t* received_data);
W25Q80DV_StatusTypeDef W25Q80DV_ReadStatusRegister(uint8_t* data);
W25Q80DV_StatusTypeDef W25Q80DV_EraseSector(uint32_t init_pos);
W25Q80DV_StatusTypeDef W25Q80DV_WriteSector(uint32_t init_pos, uint8_t* data);

#endif /* W25Q80DV_H_ */
