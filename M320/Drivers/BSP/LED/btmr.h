/**
 * @file btmr.c
 * @author JackeyZ
 * @brief 定时器相关初始化函数
 * @version 0.1
 * @date 2024-01-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __BTMR_H
#define __BTMR_H

#include "./SYSTEM/sys/sys.h"

/* GPS定义 */
extern volatile uint32_t time_s;                  // 用于计算GPS更新RTC时间以后的秒数（跨文件变量）
extern volatile uint32_t time6_ms;                // 定时器6时钟计数器（跨文件变量）

/* 基本定时器定义 */
#define BTMR_TMRX_INT               TMR6
#define BTMR_TMRX_INT_IRQn          TMR6_DAC_IRQn
#define BTMR_TMRX_INT_IRQHandler    TMR6_DAC_IRQHandler
#define BTMR_TMRX_INT_CLK_ENABLE()  do{ RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR6); }while(0)

/* 函数声明 */
void TIMERx_Init(TMR_T* TIMERx,uint16_t arr, uint16_t psc);    // 定时器初始化
void InitTimers();                                             // 所有定时器初始化

#endif
