/**
 * @file usart.c
 * @author JackeyZ
 * @brief (1) ����:		���ڴ�����ģ����ص�����
 * @version 0.1
 * @date 2023-12-28
 * 
 * @copyright Copyright (c) 2023 SEEKWAY
 * 
 */

#ifndef _USART_H
#define _USART_H

#include "includes.h"

/* ����2�жϷ����ӳ������ڽ���GPS���� */	
#define GPS_BUF_SIZE 64                           // GPS��BUF�����������Ϊ64
extern volatile uint8_t  Time2SendGPSData;        // ����GPS���ݵ�PC
extern volatile uint8_t  gps_index;               // GPS����������������
extern volatile uint8_t  GPS_Buf[GPS_BUF_SIZE];   // GPS�����������ļ�ʹ�ã�


/* ���źʹ��ڶ��� */
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

/* ���ڽ�����ض��� */
#define USART_EN_RX                 1           /* ʹ�ܴ��ڽ��� */
#define USART_REC_LEN               200         /* ����������200�ֽ����� */
#define RXBUFFERSIZE                1           /* �����С */

/* ������ر��� */
extern uint8_t g_usart_rx_buf[USART_REC_LEN];   /* ���ڽ��ջ����� */
extern uint16_t g_usart_rx_sta;                 /* ���ڽ���״̬ */
extern uint8_t g_serialRxPacket[];

/* �������� */
void USARTx_Send_Char(USART_T* USARTx,u8 ch);                       /* ���ڷ���8λ���� */
void USARTx_Send_String(USART_T* USARTx,char *str);                 /* ���ڷ����ַ��� */
void USARTx_Send_Buff(USART_T* USARTx,u8 *data_buff,u16 data_len);  /* ���ڷ������� */
void USARTx_Init(USART_T *USARTx,uint32_t baudrate);                /* ���ڳ�ʼ������ */
void InitUsarts(void);                                              /* ���д��ڳ�ʼ������ */

#endif
