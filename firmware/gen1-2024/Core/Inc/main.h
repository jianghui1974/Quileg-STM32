/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
// 初始化OLED显示
void OLED_InitDisplay(void);

//点亮LED灯
void lighting_up_LED(void);

// 更新鼠标滚动值
void OLED_UpdateScroll(int hScroll, int vScroll);

// 更新鼠标按键状态
void OLED_UpdateButtons(int btnLeft, int btnRight, int btnMiddle);

// 更新鼠标滚轮状态
void OLED_UpdateWheel(int wheel);

// 更新键盘按键
//void OLED_UpdateKey(const char* key_oled_show);
void OLED_UpdateKey(uint8_t modifier, uint8_t key1, uint8_t key2);

// 初始化鼠标键盘预设函数
void initializeMouseKeyBoards();

// 处理串口收到的鼠标数据
void processMouseCommand(uint8_t* packet);

// 处理串口收到的键盘数据
void processKeyboardCommand(uint8_t* packet);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
