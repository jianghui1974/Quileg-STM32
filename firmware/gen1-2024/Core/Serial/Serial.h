/*
 * Serial.h
 *
 *  Created on: Jun 9, 2024
 *      Author: huijiang
 */

#ifndef SERIAL_SERIAL_H_
#define SERIAL_SERIAL_H_

#include <stdio.h>

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);
void Serial_SendPacket(void);

uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxData(void);
uint8_t Serial_GetRxPacketFlag(void);
extern uint8_t Serial_TxPacket[]; // 声明发送数据包数组的外部引用
extern uint8_t Serial_RxPacket[]; // 定义接收数据包数组的外部引用

#endif /* SERIAL_SERIAL_H_ */
