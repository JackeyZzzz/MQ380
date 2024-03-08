/**
 ****************************************************************************************************
 * @file        sys.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-15
 * @brief       系统初始化代码（包括时钟配置、中断管理等）
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

#ifndef _SYS_H
#define _SYS_H

#include "apm32f4xx.h"
#include "apm32f4xx_misc.h"

/**
 * SYS_SUPPORT_OS用于定义是否支持OS
 * 0，不支持OS
 * 1，支持OS
 */
#define SYS_SUPPORT_OS  0

/* 函数声明 */
void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset);                         /* 设置中断向量表偏移地址 */
void sys_wfi_set(void);                                                                     /* 执行WFI指令(执行完该指令进入睡眠模式, 等待中断唤醒) */
void sys_intx_disable(void);                                                                /* 禁用除了NMI和Hard Fault之外的所有异常 */
void sys_intx_enable(void);                                                                 /* 启用所有异常 */
void sys_msr_msp(uint32_t addr);                                                            /* 设置栈顶地址 */
void sys_standby(void);                                                                     /* 进入待机模式 */
void sys_soft_reset(void);                                                                  /* 系统软复位 */
uint8_t sys_apm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq);   /* 配置系统时钟 */

#endif
