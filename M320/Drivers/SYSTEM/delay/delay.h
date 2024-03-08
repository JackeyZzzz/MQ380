/**
 ****************************************************************************************************
 * @file        delay.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-15
 * @brief       ʹ��SysTick����ͨ����ģʽ���ӳٽ��й���֧��uC/OS��
 *              �ṩdelay_init��ʼ����������delay_us��delay_ms����ʱ����
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

#ifndef __DELAY_H
#define __DELAY_H

#include "./SYSTEM/sys/sys.h"

/* �������� */
void delay_init(uint16_t sysclk);   /* ��ʼ����ʱ���� */
void delay_us(uint32_t nus);        /* ΢�뼶��ʱ */
void delay_ms(uint16_t nms);        /* ���뼶��ʱ */

#endif
