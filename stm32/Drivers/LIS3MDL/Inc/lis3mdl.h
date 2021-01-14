/*
 * lis3mdl.h
 *
 *  Created on: Jan 13, 2021
 *      Author: fdominguez
 */

#ifndef LIS3MDL_H_
#define LIS3MDL_H_

#include "lis3mdl_conf.h"

#define LIS3MDL_WHO_AM_I		0x0F
#define LIS3MDL_WHO_AM_I_RET	0b00111101
#define LIS3MDL_CTRL_REG1		0x20
#define LIS3MDL_CTRL_REG2		0x21
#define LIS3MDL_CTRL_REG3		0x22
#define LIS3MDL_CTRL_REG4		0x23
#define LIS3MDL_CTRL_REG5		0x24

#define LIS3MDL_OUT_X_L			0x28
#define LIS3MDL_OUT_X_H			0x29
#define LIS3MDL_OUT_Y_L			0x2A
#define LIS3MDL_OUT_Y_H			0x2B
#define LIS3MDL_OUT_Z_L			0x2C
#define LIS3MDL_OUT_Z_H			0x2D
#define LIS3MDL_TEMP_OUT_L		0x2E
#define LIS3MDL_TEMP_OUT_H		0x2F

#define LIS3MDL_INIT_RETRIALS	4

typedef enum
{
  LIS3MDL_ERROR = -1,
  LIS3MDL_OK    = 0
} LIS3MDL_StatusTypeDef;


typedef struct
{
int16_t mag_x;
int16_t mag_y;
int16_t mag_z;
int16_t temp;
} LIS3MDL_DataTypeDef;

typedef union
{
  uint8_t chars;
  struct
  {
	uint8_t TEMP_EN: 1;
	uint8_t OM: 2;
	uint8_t DO: 3;
	uint8_t FAST_ODR: 1;
	uint8_t ST: 1;
  };
} LIS3MDL_CtrlReg1TypeDef;

/* ZEROX: all bits must be set to 0 */
typedef union
{
  uint8_t chars;
  struct
  {
	uint8_t ZERO1: 1;
	uint8_t FS: 2;
	uint8_t ZERO2: 1;
	uint8_t REBOOT: 1;
	uint8_t SOFT_RST: 1;
	uint8_t ZERO3: 2;
  };
} LIS3MDL_CtrlReg2TypeDef;

/* ZEROX: must be set to 0 */
typedef union
{
  uint8_t chars;
  struct
  {
	uint8_t ZERO1: 2;
	uint8_t LP: 1;
	uint8_t ZERO2: 2;
	uint8_t SIM: 1;
	uint8_t MD: 2;
  };
} LIS3MDL_CtrlReg3TypeDef;

/* ZEROX: must be set to 0 */
typedef union
{
  uint8_t chars;
  struct
  {
	uint8_t ZERO1: 4;
	uint8_t OMZ: 2;
	uint8_t BLE: 1;
	uint8_t ZERO2: 1;
  };
} LIS3MDL_CtrlReg4TypeDef;

/* ZEROX: must be set to 0 */
typedef union
{
  uint8_t chars;
  struct
  {
	uint8_t FAST_READ: 1;
	uint8_t BDU: 1;
	uint8_t ZERO1: 6;
  };
} LIS3MDL_CtrlReg5TypeDef;

LIS3MDL_StatusTypeDef LIS3MDL_Init(void);

void LIS3MDL_ReadValues(LIS3MDL_DataTypeDef *data);

#endif /* LIS3MDL_H_ */
