/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"
#include "usart.h"
#include "w25q80dv.h"
#include "lis3mdl.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* ID + x_mag + y_mag + z_mag + temp = 12 bytes = 12 * 8 bits */
#define FLASH_BITS_STORED_PER_DATA	12 * 8
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern osMessageQId UARTQueueHandle;
extern osSemaphoreId SPISemaphoreHandle;
/* USER CODE END Variables */
osThreadId UARTTaskHandle;
osThreadId MagTaskHandle;
osMessageQId UARTQueueHandle;
osSemaphoreId SPISemaphoreHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartUARTTask(void const * argument);
void StartMagTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of SPISemaphore */
  osSemaphoreDef(SPISemaphore);
  SPISemaphoreHandle = osSemaphoreCreate(osSemaphore(SPISemaphore), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of UARTQueue */
  osMessageQDef(UARTQueue, 1, uint32_t);
  UARTQueueHandle = osMessageCreate(osMessageQ(UARTQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of UARTTask */
  osThreadDef(UARTTask, StartUARTTask, osPriorityAboveNormal, 0, 128);
  UARTTaskHandle = osThreadCreate(osThread(UARTTask), NULL);

  /* definition and creation of MagTask */
  osThreadDef(MagTask, StartMagTask, osPriorityNormal, 0, 128);
  MagTaskHandle = osThreadCreate(osThread(MagTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartUARTTask */
/**
  * @brief  Function implementing the UARTTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartUARTTask */
void StartUARTTask(void const * argument)
{
  /* USER CODE BEGIN StartUARTTask */
  char rx_buffer[UART_DATA_SIZE];
  char read_data[4], dt_buff[6];
  osEvent event;
  float x_mag, y_mag, z_mag, temp_mag;
  int received_id_value;

  /* Start DMA RX interrupt (circular mode) */
  HAL_UART_Receive_DMA(&huart1,(uint8_t*)&rx_buffer[0],UART_DATA_SIZE);

  /* Infinite loop */
  for(;;)
  {
	/* Wait until UART data received */
	event = osMessageGet(UARTQueueHandle, osWaitForever);
	if(event.status == osEventMessage)
	{
 	  /* Take SPI semaphore when available */
	  if(osSemaphoreWait(SPISemaphoreHandle, osWaitForever) > 0)
	  {
		/* Get ID value */
		received_id_value = atoi(rx_buffer);

		W25Q80DV_ReadBytes(received_id_value * FLASH_BITS_STORED_PER_DATA, (uint8_t*)read_data, 4);

		if(atoi(read_data) != received_id_value)
		{
		  SERIAL_SEND("Error. Data not found.\r\n");
		}
		else
		{
		  W25Q80DV_ReadBytes((received_id_value * FLASH_BITS_STORED_PER_DATA + 32), (uint8_t*)read_data, 4);
		  x_mag = atof(read_data);
		  SERIAL_SEND("Magnetometer x value = ");
		  sprintf(dt_buff,"%4.2f", x_mag);
		  SERIAL_SEND(dt_buff);SERIAL_SEND("\r\n");

		  W25Q80DV_ReadBytes((received_id_value * FLASH_BITS_STORED_PER_DATA + 64), (uint8_t*)read_data, 4);
		  y_mag = atof(read_data);
		  SERIAL_SEND("Magnetometer y value = ");
		  sprintf(dt_buff,"%4.2f", y_mag);
		  SERIAL_SEND(dt_buff);SERIAL_SEND("\r\n");

		  W25Q80DV_ReadBytes((received_id_value * FLASH_BITS_STORED_PER_DATA + 96), (uint8_t*)read_data, 4);
		  z_mag = atof(read_data);
		  SERIAL_SEND("Magnetometer z value = ");
		  sprintf(dt_buff,"%4.2f", z_mag);
		  SERIAL_SEND(dt_buff);SERIAL_SEND("\r\n");

		  W25Q80DV_ReadBytes((received_id_value * FLASH_BITS_STORED_PER_DATA + 128), (uint8_t*)read_data, 4);
		  temp_mag = atof(read_data);
		  SERIAL_SEND("Temperature value = ");
		  sprintf(dt_buff,"%4.2f", temp_mag);
		  SERIAL_SEND(dt_buff);SERIAL_SEND("\r\n");
		}

		/* Release SPI semaphore */
		osSemaphoreRelease(SPISemaphoreHandle);
	  }
    }
  }
  /* USER CODE END StartUARTTask */
}

/* USER CODE BEGIN Header_StartMagTask */
/**
* @brief Function implementing the MagTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMagTask */
void StartMagTask(void const * argument)
{
  /* USER CODE BEGIN StartMagTask */
  LIS3MDL_DataTypeDef read_data;
  uint32_t memory_offset = 0;
  uint8_t sector_data[W25Q80DV_SECTOR_SIZE];

  /* Infinite loop */
  for(;;)
  {
    /* Take SPI semaphore when available */
    if(osSemaphoreWait(SPISemaphoreHandle, osWaitForever) > 0)
    {

      /* FIXME: Check all of this */

      /* Increment memory offset */
      /* Read magnetometer values */
	  LIS3MDL_ReadValues(&read_data);

	  /* First, read data of target sector*/
	  W25Q80DV_ReadSector((memory_offset & (0xFFFFF000)) * FLASH_BITS_STORED_PER_DATA, sector_data);

	  /* Then, erase sector */
	  W25Q80DV_EraseSector(memory_offset * FLASH_BITS_STORED_PER_DATA);

	  /* Finally, write the new values and write the sector in memory */
	  sector_data[(memory_offset & (0xFFF)) * FLASH_BITS_STORED_PER_DATA] = memory_offset;
	  sector_data[(memory_offset & (0xFFF)) * FLASH_BITS_STORED_PER_DATA + 32] = read_data.mag_x;
	  sector_data[(memory_offset & (0xFFF)) * FLASH_BITS_STORED_PER_DATA + 48] = read_data.mag_y;
	  sector_data[(memory_offset & (0xFFF)) * FLASH_BITS_STORED_PER_DATA + 64] = read_data.mag_z;
	  sector_data[(memory_offset & (0xFFF)) * FLASH_BITS_STORED_PER_DATA + 80] = read_data.temp;
    }
  }
  /* USER CODE END StartMagTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
