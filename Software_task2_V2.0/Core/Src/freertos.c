/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "tim.h"
#include "queue.h"
#include "usart.h"
#include "stdio.h"
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
	extern int pwmval;
	extern int task1_flag;
	extern int task2_flag;
	extern int IRQ_flag;
	extern int Callback_flag;
	extern char rxBUFF[];
	uint32_t rx_counter = 0;
	int rx_complete = 0;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for LED_Breath */
osThreadId_t LED_BreathHandle;
const osThreadAttr_t LED_Breath_attributes = {
  .name = "LED_Breath",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow1,
};
/* Definitions for Uart_Receive */
osThreadId_t Uart_ReceiveHandle;
const osThreadAttr_t Uart_Receive_attributes = {
  .name = "Uart_Receive",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow2,
};
/* Definitions for Uart_Queue */
osMessageQueueId_t Uart_QueueHandle;
const osMessageQueueAttr_t Uart_Queue_attributes = {
  .name = "Uart_Queue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void DMA_TransferCompleteCallback(DMA_HandleTypeDef *hdma);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void LED_BreathTask(void *argument);
void Uart_ReceiveTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	HAL_DMA_RegisterCallback(&hdma_usart1_rx, HAL_DMA_XFER_CPLT_CB_ID, DMA_TransferCompleteCallback);
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of Uart_Queue */
  Uart_QueueHandle = osMessageQueueNew (1, sizeof(uint16_t), &Uart_Queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of LED_Breath */
  LED_BreathHandle = osThreadNew(LED_BreathTask, NULL, &LED_Breath_attributes);

  /* creation of Uart_Receive */
  Uart_ReceiveHandle = osThreadNew(Uart_ReceiveTask, NULL, &Uart_Receive_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  while(1)
	{
//		task3_flag++;
		osDelay(100);
	}
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_LED_BreathTask */
/**
* @brief Function implementing the LED_Breath thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LED_BreathTask */
void LED_BreathTask(void *argument)
{
  /* USER CODE BEGIN LED_BreathTask */
  /* Infinite loop */
  while(1)
  {
	  task1_flag++;
		  while(pwmval<500)
	  {
		  pwmval++;
		  __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,pwmval);
		  osDelay(5);
	  }
	  while(pwmval)
	  {
		  pwmval--;
		  __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,pwmval);
		  osDelay(5);
	  }
  }
  /* USER CODE END LED_BreathTask */
}

/* USER CODE BEGIN Header_Uart_ReceiveTask */
/**
* @brief Function implementing the Uart_Receive thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Uart_ReceiveTask */
void Uart_ReceiveTask(void *argument)
{
  /* USER CODE BEGIN Uart_ReceiveTask */
  /* Infinite loop */
  // 从队列中获取数据并发送到电脑
   while(1)
   {
	   uint8_t rxData[100];
		task2_flag++;
		if (xQueueReceive(Uart_QueueHandle, &rxData, portMAX_DELAY) == pdTRUE)
		{
		  // 发送数据到电脑
		  HAL_UART_Transmit(&huart1, rxData, 100, HAL_MAX_DELAY);
		}
   }
	
  /* USER CODE END Uart_ReceiveTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
//void HAL_UART_IdleCpltCallback(UART_HandleTypeDef *huart)
//{
//    Callback_flag=10;
//	extern char rxBUFF[];
//	printf("%s\r\n",rxBUFF);
//    if(huart->Instance==USART1)
//	{	
//		if (__HAL_DMA_GET_COUNTER(huart->hdmarx) == 0)
//		{
//			uint32_t DMA_BUFFER_SIZE = 100 - huart->hdmarx->Instance->CNDTR;
//		  // 判断是否接收到回车符
//		  if (rxBUFF[DMA_BUFFER_SIZE - 1] == '\n')
//		  {
//			// 将接收到的数据通过队列传输到任务			
//			  xQueueSend(Uart_QueueHandle, &rxBUFF, portMAX_DELAY);	
//				Callback_flag=20;
//		  }
//		}
//	}
//}

void DMA_TransferCompleteCallback(DMA_HandleTypeDef *hdma) 
{
	Callback_flag=10;
	if(hdma == &hdma_usart1_rx)
  {
    /* Get the length of the received data */
    uint16_t length = 100- __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
    
    /* Process the received data */
    // Send data to the computer or do other operations
    xQueueSend(Uart_QueueHandle, &rxBUFF, portMAX_DELAY);
    /* Restart the DMA reception */
    HAL_UART_Receive_DMA(&huart1,(u8*)rxBUFF, 100);
	  
  }
}
/* USER CODE END Application */

