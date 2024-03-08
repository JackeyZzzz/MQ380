/**
 * @file btmr.c
 * @author JackeyZ
 * @brief ��ʱ����س�ʼ������
 * @version 0.1
 * @date 2024-01-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __BTMR_H
#define __BTMR_H

#include "./SYSTEM/sys/sys.h"

/* GPS���� */
extern volatile uint32_t time_s;                  // ���ڼ���GPS����RTCʱ���Ժ�����������ļ�������
extern volatile uint32_t time6_ms;                // ��ʱ��6ʱ�Ӽ����������ļ�������

/* ������ʱ������ */
#define BTMR_TMRX_INT               TMR6
#define BTMR_TMRX_INT_IRQn          TMR6_DAC_IRQn
#define BTMR_TMRX_INT_IRQHandler    TMR6_DAC_IRQHandler
#define BTMR_TMRX_INT_CLK_ENABLE()  do{ RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR6); }while(0)

/* �������� */
void TIMERx_Init(TMR_T* TIMERx,uint16_t arr, uint16_t psc);    // ��ʱ����ʼ��
void InitTimers();                                             // ���ж�ʱ����ʼ��

#endif
