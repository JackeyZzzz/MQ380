/**
 ****************************************************************************************************
 * @file        delay.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-15
 * @brief       使用SysTick的普通计数模式对延迟进行管理（支持uC/OS）
 *              提供delay_init初始化函数，和delay_us和delay_ms等延时函数
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 * 
 * 实验平台:正点原子 APM32F407最小系统板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 * 
 ****************************************************************************************************
 */

#ifndef __DELAY_H
#define __DELAY_H

#include "./SYSTEM/sys/sys.h"

/* 函数声明 */
void delay_init(uint16_t sysclk);   /* 初始化延时功能 */
void delay_us(uint32_t nus);        /* 微秒级延时 */
void delay_ms(uint16_t nms);        /* 毫秒级延时 */

#endif
