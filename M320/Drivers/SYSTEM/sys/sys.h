/**
 ****************************************************************************************************
 * @file        sys.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-15
 * @brief       ϵͳ��ʼ�����루����ʱ�����á��жϹ���ȣ�
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 * 
 * ʵ��ƽ̨:����ԭ�� APM32F407��Сϵͳ��
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 * 
 ****************************************************************************************************
 */

#ifndef _SYS_H
#define _SYS_H

#include "apm32f4xx.h"
#include "apm32f4xx_misc.h"

/**
 * SYS_SUPPORT_OS���ڶ����Ƿ�֧��OS
 * 0����֧��OS
 * 1��֧��OS
 */
#define SYS_SUPPORT_OS  0

/* �������� */
void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset);                         /* �����ж�������ƫ�Ƶ�ַ */
void sys_wfi_set(void);                                                                     /* ִ��WFIָ��(ִ�����ָ�����˯��ģʽ, �ȴ��жϻ���) */
void sys_intx_disable(void);                                                                /* ���ó���NMI��Hard Fault֮��������쳣 */
void sys_intx_enable(void);                                                                 /* ���������쳣 */
void sys_msr_msp(uint32_t addr);                                                            /* ����ջ����ַ */
void sys_standby(void);                                                                     /* �������ģʽ */
void sys_soft_reset(void);                                                                  /* ϵͳ��λ */
uint8_t sys_apm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq);   /* ����ϵͳʱ�� */

#endif
