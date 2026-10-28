/*
 * Key.c
 *
 *  Created on: Jun 12, 2024
 *      Author: huijiang
 */

#include "stm32f1xx_hal.h"  // 根据你的具体STM32型号，包含相应的HAL库头文件
#include "Key.h"

/**
  * 函    数：按键初始化
  * 参    数：无
  * 返 回 值：无
  */
void Key_Init(void)
{
    /*开启时钟*/
    __HAL_RCC_GPIOB_CLK_ENABLE();  // 开启GPIOB的时钟

    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_12 | GPIO_PIN_14;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);  // 将PB0和PB11引脚初始化为上拉输入
}

/**
  * 函    数：按键获取键码
  * 参    数：无
  * 返 回 值：按下按键的键码值，范围：0~2，返回0代表没有按键按下
  * 注意事项：此函数是阻塞式操作，当按键按住不放时，函数会卡住，直到按键松手
  */
uint8_t Key_GetNum(void)
{
    uint8_t KeyNum = 255;  // 定义变量，默认键码值为255

    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET)  // 读PB10输入寄存器的状态，如果为0，则代表按键1按下
    {
        HAL_Delay(20);  // 延时消抖
        while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET);  // 等待按键松手
        HAL_Delay(20);  // 延时消抖
        KeyNum = 10;  // 置键码为10
    }

    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET)  // 读PB11输入寄存器的状态，如果为0，则代表按键1按下
    {
        HAL_Delay(20);  // 延时消抖
        while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET);  // 等待按键松手
        HAL_Delay(20);  // 延时消抖
        KeyNum = 11;  // 置键码为11
    }

    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET)  // 读PB5输入寄存器的状态，如果为0，则代表按键1按下
    {
        HAL_Delay(20);  // 延时消抖
        while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET);  // 等待按键松手
        HAL_Delay(20);  // 延时消抖
        KeyNum = 5;  // 置键码为5
    }

    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == GPIO_PIN_RESET)  // 读PB6输入寄存器的状态，如果为0，则代表按键1按下
    {
        HAL_Delay(20);  // 延时消抖
        while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == GPIO_PIN_RESET);  // 等待按键松手
        HAL_Delay(20);  // 延时消抖
        KeyNum = 6;  // 置键码为6
    }

    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_RESET)  // 读PB7输入寄存器的状态，如果为0，则代表按键1按下
    {
        HAL_Delay(20);  // 延时消抖
        while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_RESET);  // 等待按键松手
        HAL_Delay(20);  // 延时消抖
        KeyNum = 7;  // 置键码为7
    }

    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET)  // 读PB12输入寄存器的状态，如果为0，则代表按键1按下
    {
        HAL_Delay(20);  // 延时消抖
        while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET);  // 等待按键松手
        HAL_Delay(20);  // 延时消抖
        KeyNum = 12;  // 置键码为12
    }

    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET)  // 读PB14输入寄存器的状态，如果为0，则代表按键1按下
    {
        HAL_Delay(20);  // 延时消抖
        while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET);  // 等待按键松手
        HAL_Delay(20);  // 延时消抖
        KeyNum = 14;  // 置键码为14
    }


    return KeyNum;  // 返回键码值，如果没有按键按下，所有if都不成立，则键码为默认值0
}
