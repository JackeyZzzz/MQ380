/**
 * @file rs485.h
 * @author Jackey
 * @brief (1) 功能:		用于处理485模块相关数据的头文件
 * @version 0.1
 * @date 2023-12-28
 * 
 * @copyright Copyright (c) 2023 SEEKWAY
 * 
 */
#ifndef __rs485_H
#define __rs485_H
#include "apm32f4xx_rcm.h"

/* RS485接收数据变量（跨文件变量） */
extern uint8_t RS485_RX_BUF[];  	//接收缓冲,最大64个字节.
extern u8 RS485_RX_CNT;   			//接收到的数据长度

/**
 * @brief RS485(串口6读写片选)
 * 
 */
#define RS485_TX_EN(x)                 do{  x ?                                   \
                                            GPIO_ResetBit(GPIOC, GPIO_PIN_8) :    \
                                            GPIO_SetBit(GPIOC, GPIO_PIN_8);       \
                                         }while(0)

/**
 * @brief RS485（串口5读写片选）
 * 
 */
#define RS485_TX1_EN(x)                 do{ x ?                                   \
                                            GPIO_ResetBit(GPIOC, GPIO_PIN_10) :   \
                                            GPIO_SetBit(GPIOC, GPIO_PIN_10);      \
                                          }while(0)

/* RS485自定义函数 */
void InitRs485s(void);                         // RS485所有串口初始化函数
void InitRs485(USART_T *USARTx,u32 baudrate);  // RS485单个初始化
void RS485_Send_Data(u8 *buf,u8 len);          // RS485发送数据函数
void RS485_Receive_Data(u8 *buf,u8 *len);      // RS485接收数据函数                            
void DmxHandleTask(void);                      // DMX处理函数（放在主函数中）
void SetDmxRevFinish(u8 val);                  // 设置Dmx接收完成标志位
u8 GetDmxRevFinish(void);                      // 获取Dmx接收完成标志位

#endif
