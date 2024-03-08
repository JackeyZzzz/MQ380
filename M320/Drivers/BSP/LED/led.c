/**
 * @file led.c
 * @author JackeyZ
 * @brief (1) ����:		LED��ʼ��
 * @version 0.1
 * @date 2023-12-28
 * 
 * @copyright Copyright (c) 2023 SEEKWAY
 * 
 */

#include "./BSP/LED/led.h"
#include "apm32f4xx_rcm.h"

/**
 * @brief       ��ʼ��LED 
 * @note        1��LED0����ΪGPIOC��GPIO_PIN_9 
 *              2��LED1����ΪGPIOF��GPIO_PIN_10
 * @param       ��
 * @retval      ��
 */
void InitLed(void)
{
    GPIO_Config_T gpio_init_struct;
    
    /* ʹ��ʱ�� */
    LED0_GPIO_CLK_ENABLE();
    LED1_GPIO_CLK_ENABLE();
    
    /* ����LED0���� */
    gpio_init_struct.pin = LED0_GPIO_PIN;           /* LED0���� */
    gpio_init_struct.mode = GPIO_MODE_OUT;          /* ���ģʽ */
    gpio_init_struct.speed = GPIO_SPEED_100MHz;     /* ���� */
    gpio_init_struct.otype = GPIO_OTYPE_PP;         /* ������� */
    gpio_init_struct.pupd = GPIO_PUPD_DOWN;         /* ���� */
    GPIO_Config(LED0_GPIO_PORT, &gpio_init_struct); /* ����LED0���� */
    
    /* ����LED1���� */
    gpio_init_struct.pin = LED1_GPIO_PIN;           /* LED1���� */
    gpio_init_struct.mode = GPIO_MODE_OUT;          /* ���ģʽ */
    gpio_init_struct.speed = GPIO_SPEED_100MHz;     /* ���� */
    gpio_init_struct.otype = GPIO_OTYPE_PP;         /* ������� */
    gpio_init_struct.pupd = GPIO_PUPD_DOWN;         /* ���� */
    GPIO_Config(LED1_GPIO_PORT, &gpio_init_struct); /* ����LED1���� */
    
    /* Ĭ�Ϲر�����LED */
    LED0(1);
    LED1(1);
}

