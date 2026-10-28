/*
 * Serial.c
 *
 *  Created on: Jun 9, 2024
 *      Author: huijiang
 */

#include "stm32f1xx_hal.h"  // 根据你的具体STM32型号，包含相应的HAL库头文件
#include <stdio.h>
#include <stdarg.h>
#include "oled.h" //显示屏

uint8_t Serial_RxData;		//定义串口接收的数据变量
uint8_t Serial_RxFlag;		//定义串口接收的标志位变量
UART_HandleTypeDef huart_x;
uint8_t Serial_TxPacket[10];				//定义发送数据包数组，数据包格式：FF 01 02 03 04 ... 0A FE
uint8_t Serial_RxPacket[10];				//定义接收数据包数组
uint8_t Serial_RxPacketFlag;		//定义串口包接收的标志位变量

/**
  * 函    数：串口初始化
  * 参    数：无
  * 返 回 值：无
  */
void Serial_Init(void)
{
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    /* UART1 GPIO Configuration
       PA9     ------> USART1_TX
       PA10    ------> USART1_RX
    */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 init */
    huart_x.Instance = USART1;
    huart_x.Init.BaudRate = 9600;
    huart_x.Init.WordLength = UART_WORDLENGTH_8B;
    huart_x.Init.StopBits = UART_STOPBITS_1;
    huart_x.Init.Parity = UART_PARITY_NONE;
    huart_x.Init.Mode = UART_MODE_TX_RX;
    huart_x.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart_x.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart_x) != HAL_OK)
    {
        /* Initialization Error */
//        Error_Handler();
    }

    /* NVIC configuration for USART1 interrupt */
    HAL_NVIC_SetPriority(USART1_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(&huart_x, UART_IT_RXNE);

    /* Initialize the UART Receive Interrupt */
    HAL_UART_Receive_IT(&huart_x, &Serial_RxData, 1);
}

/**
  * 函    数：串口发送一个字节
  * 参    数：Byte 要发送的一个字节
  * 返 回 值：无
  */
void Serial_SendByte(uint8_t Byte)
{
    HAL_UART_Transmit(&huart_x, &Byte, 1, HAL_MAX_DELAY); // 使用 HAL 库发送字节数据
}


/**
  * 函    数：串口发送一个数组
  * 参    数：Array 要发送数组的首地址
  * 参    数：Length 要发送数组的长度
  * 返 回 值：无
  */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
    HAL_UART_Transmit(&huart_x, Array, Length, HAL_MAX_DELAY); // 使用 HAL 库发送数组数据
}

/**
  * 函    数：串口发送一个字符串
  * 参    数：String 要发送字符串的首地址
  * 返 回 值：无
  */
void Serial_SendString(char *String)
{
    while(*String)
    {
        Serial_SendByte((uint8_t)(*String++)); // 发送字符串中的每一个字节
    }
}

/**
  * 函    数：次方函数（内部使用）
  * 返 回 值：返回值等于X的Y次方
  */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--)
    {
        Result *= X;
    }
    return Result;
}

/**
  * 函    数：串口发送数字
  * 参    数：Number 要发送的数字，范围：0~4294967295
  * 参    数：Length 要发送数字的长度，范围：0~10
  * 返 回 值：无
  */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        Serial_SendByte((Number / Serial_Pow(10, Length - i - 1) % 10) + '0');
    }
}

/**
  * 函    数：使用printf需要重定向的底层函数
  * 参    数：保持原始格式即可，无需变动
  * 返 回 值：保持原始格式即可，无需变动
  */
int fputc(int ch, FILE *f)
{
    Serial_SendByte((uint8_t)ch);
    return ch;
}

/**
  * 函    数：自己封装的prinf函数
  * 参    数：format 格式化字符串
  * 参    数：... 可变的参数列表
  * 返 回 值：无
  */
void Serial_Printf(char *format, ...)
{
    char String[100];
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    Serial_SendString(String);
}

/**
  * 函    数：串口发送数据包
  * 参    数：无
  * 返 回 值：无
  * 说    明：调用此函数后，Serial_TxPacket数组的内容将加上包头（FF）包尾（FE）后，作为数据包发送出去
  */
void Serial_SendPacket(void)
{
	Serial_SendByte(0xFF);
	Serial_SendArray(Serial_TxPacket, 10);
	Serial_SendByte(0xFE);
}

/**
  * 函    数：获取串口接收标志位
  * 参    数：无
  * 返 回 值：串口接收标志位，范围：0~1，接收到数据后，标志位置1，读取后标志位自动清零
  */
uint8_t Serial_GetRxFlag(void)
{
    if (Serial_RxFlag == 1)
    {
        Serial_RxFlag = 0;
        return 1;
    }
    return 0;
}

/**
  * 函    数：获取串口包接收标志位
  * 参    数：无
  * 返 回 值：串口接收标志位，范围：0~1，接收到数据后，标志位置1，读取后标志位自动清零
  */
uint8_t Serial_GetRxPacketFlag(void)
{
    if (Serial_RxPacketFlag == 1)
    {
    	Serial_RxPacketFlag = 0;
        return 1;
    }
    return 0;
}



/**
  * 函    数：获取串口接收的数据
  * 参    数：无
  * 返 回 值：接收的数据，范围：0~255
  */
uint8_t Serial_GetRxData(void)
{
    return Serial_RxData;
}

// 在 HAL 库中，中断处理的代码通常放在回调函数中
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        Serial_RxData = huart->Instance->DR; // 读取数据寄存器
        Serial_RxFlag = 1;                   // 置接收标志位变量为1

        // 重新启动接收中断，如果需要继续接收数据
        HAL_UART_Receive_IT(&huart_x, &Serial_RxData, 1);
    }
}

/**
  * 函    数：USART1中断函数
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
  *           函数名为预留的指定名称，可以从启动文件复制
  *           请确保函数名正确，不能有任何差异，否则中断函数将不能进入
  */
void USART1_IRQHandler(void)
{
	uint8_t this_RxData;
    static uint8_t this_RxState = 0;     // 定义表示当前状态机状态的静态变量
    static uint8_t this_pRxPacket = 0;   // 定义表示当前接收数据位置的静态变量

    // 检查是否是USART1的接收事件触发的中断
    if (__HAL_UART_GET_FLAG(&huart_x, UART_FLAG_RXNE) != RESET)
    {
        // 读取数据寄存器，存放在接收的数据变量
        this_RxData = (uint8_t)(huart_x.Instance->DR & 0x00FF);

        /* 使用状态机的思路，依次处理数据包的不同部分 */
        if (this_RxState == 0) // 当前状态为0，接收数据包包头
        {
            if (this_RxData == 0xFF) // 如果数据确实是包头
            {
            	this_RxState = 1;   // 置下一个状态
            	this_pRxPacket = 0; // 数据包的位置归零
            }
        }
        else if (this_RxState == 1) // 当前状态为1，接收数据包数据
        {
            Serial_RxPacket[this_pRxPacket] = this_RxData; // 将数据存入数据包数组的指定位置
            this_pRxPacket++; // 数据包的位置自增
            if (this_pRxPacket >= 10) // 如果收够10个数据
            {
            	this_RxState = 2; // 置下一个状态
            }
        }
        else if (this_RxState == 2) // 当前状态为2，接收数据包包尾
        {
            if (this_RxData == 0xFE) // 如果数据确实是包尾部
            {
            	this_RxState = 0;        // 状态归0
            	Serial_RxPacketFlag = 1; // 接收数据包标志位置1，成功接收一个数据包
            }
            else // 如果包尾部数据不正确，重置状态
            {
                this_RxState = 0;
            }
        }
    }
}
