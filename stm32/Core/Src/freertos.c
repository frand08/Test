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
#include "lis3mdl.h"
#include "w25q80dv.h"
#include "extflash_memory.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;

osThreadId MagTaskHandle;

/* USER CODE END Variables */
osThreadId UARTTaskHandle;
osMessageQId UARTQueueHandle;
osMessageQId SPITxQueueHandle;
osMessageQId SPIRxQueueHandle;
osSemaphoreId SPISemaphoreHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void StartMagTask(void const * argument);

/* USER CODE END FunctionPrototypes */

void StartUARTTask(void const * argument);

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

  /* definition and creation of SPITxQueue */
  osMessageQDef(SPITxQueue, 1, uint32_t);
  SPITxQueueHandle = osMessageCreate(osMessageQ(SPITxQueue), NULL);

  /* definition and creation of SPIRxQueue */
  osMessageQDef(SPIRxQueue, 1, uint32_t);
  SPIRxQueueHandle = osMessageCreate(osMessageQ(SPIRxQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of UARTTask */
  osThreadDef(UARTTask, StartUARTTask, osPriorityAboveNormal, 0, 128);
  UARTTaskHandle = osThreadCreate(osThread(UARTTask), NULL);

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
  char dt_buff[30];
  osEvent event;
  int16_t x_mag, y_mag, z_mag, temp_mag;
  uint32_t received_id_value;

  /* Start DMA RX interrupt (circular mode) */
  HAL_UART_Receive_DMA(&huart1,(uint8_t*)&rx_buffer[0],UART_DATA_SIZE);

  /* Try to initialize both memory and magnetometer, if error reset program */
  if(osSemaphoreWait(SPISemaphoreHandle, osWaitForever) > 0)
  {
	  /* If memory init could not be done,
	   * inform via UART and do something (reset maybe).
	   */
	  if(W25Q80DV_Init() != W25Q80DV_OK)
	  {
		SERIAL_SEND("FLASH init error. Resetting MCU\r\n");
		/* TODO: Reset MCU or something... */
		osThreadTerminate(UARTTaskHandle);
	  }

	  /* If magnetometer init could not be done,
	   * inform via UART and do something (reset maybe).
	   */
	  if(LIS3MDL_Init() != LIS3MDL_OK)
	  {
		SERIAL_SEND("FLASH init error. Resetting MCU\r\n");
		/* TODO: Reset MCU or something... */
		osThreadTerminate(UARTTaskHandle);
	  }
  }

  /* Otherwise define and create magnetometer task */
  osThreadDef(MagTask, StartMagTask, osPriorityNormal, 0, 128);
  MagTaskHandle = osThreadCreate(osThread(MagTask), NULL);

  /* Infinite loop */
  for(;;)
  {
	/* Wait until UART data received */
	event = osMessageGet(UARTQueueHandle, osWaitForever);
	if(event.status == osEventMessage)
	{
 	  /* Take SPI semaphore when available, so that we receive the data as fast as possible */
	  if(osSemaphoreWait(SPISemaphoreHandle, osWaitForever) > 0)
	  {
		/* Get ID value */
		received_id_value = atoi(rx_buffer);

		/* Check if data present on memory, otherwise send error. */
		if(EXTFLASH_CheckID(received_id_value) != EXTFLASH_OK)
		{
		  SERIAL_SEND("Error. Data not found.\r\n");
		}
		else
		{
		  /* Get data from FLASH memory and send it via UART if OK */
		  if(EXTFLASH_ReadData(received_id_value, &x_mag, &y_mag, &z_mag, &temp_mag) == EXTFLASH_OK)
		  {
			  /* Send data via UART */
			  sprintf(dt_buff,"Magnetometer x value = %d\r\n", x_mag);
			  SERIAL_SEND(dt_buff);

			  sprintf(dt_buff,"Magnetometer y value = %d\r\n", y_mag);
			  SERIAL_SEND(dt_buff);

			  sprintf(dt_buff,"Magnetometer z value = %d\r\n", z_mag);
			  SERIAL_SEND(dt_buff);

			  sprintf(dt_buff,"Temperature value = %d\r\n", temp_mag);
			  SERIAL_SEND(dt_buff);
		  }
		  else
		  {
		    SERIAL_SEND("Error reading external flash data\r\n");
		  }
		}

		/* Release SPI semaphore */
		osSemaphoreRelease(SPISemaphoreHandle);
	  }
    }
  }
  /* USER CODE END StartUARTTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/**
  * @brief Function implementing the MagTask thread.
  * @param argument: Not used
  * @retval None
  */
void StartMagTask(void const * argument)
{
  LIS3MDL_DataTypeDef read_data;
  LIS3MDL_StatusTypeDef magnetometer_retval = LIS3MDL_ERROR;
  uint32_t memory_id = 0;

  /* Infinite loop */
  for(;;)
  {
    /* Take SPI semaphore when available */
    if(osSemaphoreWait(SPISemaphoreHandle, osWaitForever) > 0)
    {
        /* Read magnetometer values */
    	magnetometer_retval = LIS3MDL_ReadValues(&read_data);

    	/* Release SPI semaphore */
    	osSemaphoreRelease(SPISemaphoreHandle);
    }

    /* In case a UART receive interrupt occured during the LIS3MDL read, release the SPI
     * semaphore to unlock the serial thread. An osDelay is added to call the scheduler */
    osDelay(1);

    /* Try to write only if magnetometer data read OK */
    if(magnetometer_retval == LIS3MDL_OK)
    {
	  /* Take SPI semaphore when available */
	  if(osSemaphoreWait(SPISemaphoreHandle, osWaitForever) > 0)
	  {
		/* Write to FLASH memory and if OK increment offset */
		if(EXTFLASH_WriteData(memory_id, read_data.mag_x, read_data.mag_y, read_data.mag_z, read_data.temp) == EXTFLASH_OK)
		{
		  /* Increment memory offset */
		  memory_id++;
		}

		/* Release SPI semaphore */
		osSemaphoreRelease(SPISemaphoreHandle);
	  }
    }
	/* Send data every second (if possible) */
	osDelay(1000);
  }
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
