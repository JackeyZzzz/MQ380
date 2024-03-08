/**
 * @file usart.c
 * @author JackeyZ
 * @brief (1) 功能:		用于处理串口模块相关的数据
 * @version 0.1
 * @date 2023-12-28
 * 
 * @copyright Copyright (c) 2023 SEEKWAY
 * 
 */

#ifndef _USART_H
#define _USART_H

#include "includes.h"

/* 串口2中断服务子程序，用于接收GPS数据 */	
#define GPS_BUF_SIZE 64                           // GPS的BUF数组最大数量为64
extern volatile uint8_t  Time2SendGPSData;        // 发送GPS数据到PC
extern volatile uint8_t  gps_index;               // GPS缓冲区“计数器”
extern volatile uint8_t  GPS_Buf[GPS_BUF_SIZE];   // GPS缓冲区（跨文件使用）


/* 引脚和串口定义 */
#define USART_TX_GPIO_PORT          GPIOA
#define USART_TX_GPIO_PIN           GPIO_PIN_9
#define USART_TX_GPIO_PIN_SOURCE    GPIO_PIN_SOURCE_9
#define USART_TX_GPIO_AF            GPIO_AF_USART1
#define USART_TX_GPIO_CLK_ENABLE()  do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA); }while(0)

#define USART_RX_GPIO_PORT          GPIOA
#define USART_RX_GPIO_PIN           GPIO_PIN_10
#define USART_RX_GPIO_PIN_SOURCE    GPIO_PIN_SOURCE_10
#define USART_RX_GPIO_AF            GPIO_AF_USART1
#define USART_RX_GPIO_CLK_ENABLE()  do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA); }while(0)

#define USART_UX                    USART1
#define USART_UX_IRQn               USART1_IRQn
#define USART_UX_IRQHandler         USART1_IRQHandler
#define USART_UX_CLK_ENABLE()       do{ RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_USART1); }while(0)

/* 串口接收相关定义 */
#define USART_EN_RX                 1           /* 使能串口接收 */
#define USART_REC_LEN               200         /* 定义最大接收200字节数据 */
#define RXBUFFERSIZE                1           /* 缓存大小 */

/* 导出相关变量 */
extern uint8_t g_usart_rx_buf[USART_REC_LEN];   /* 串口接收缓冲区 */
extern uint16_t g_usart_rx_sta;                 /* 串口接收状态 */
extern uint8_t g_serialRxPacket[];

/* 函数声明 */
void USARTx_Send_Char(USART_T* USARTx,u8 ch);                       /* 串口发送8位数据 */
void USARTx_Send_String(USART_T* USARTx,char *str);                 /* 串口发送字符串 */
void USARTx_Send_Buff(USART_T* USARTx,u8 *data_buff,u16 data_len);  /* 串口发送数组 */
void USARTx_Init(USART_T *USARTx,uint32_t baudrate);                /* 串口初始化函数 */
void InitUsarts(void);                                              /* 所有串口初始化函数 */

#endif
