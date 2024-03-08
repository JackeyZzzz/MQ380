/**
 * @file led.c
 * @author JackeyZ
 * @brief (1) 功能:		LED初始化
 * @version 0.1
 * @date 2023-12-28
 * 
 * @copyright Copyright (c) 2023 SEEKWAY
 * 
 */

#include "./BSP/LED/led.h"
#include "apm32f4xx_rcm.h"

/**
 * @brief       初始化LED 
 * @note        1、LED0引脚为GPIOC的GPIO_PIN_9 
 *              2、LED1引脚为GPIOF的GPIO_PIN_10
 * @param       无
 * @retval      无
 */
void InitLed(void)
{
    GPIO_Config_T gpio_init_struct;
    
    /* 使能时钟 */
    LED0_GPIO_CLK_ENABLE();
    LED1_GPIO_CLK_ENABLE();
    
    /* 配置LED0引脚 */
    gpio_init_struct.pin = LED0_GPIO_PIN;           /* LED0引脚 */
    gpio_init_struct.mode = GPIO_MODE_OUT;          /* 输出模式 */
    gpio_init_struct.speed = GPIO_SPEED_100MHz;     /* 高速 */
    gpio_init_struct.otype = GPIO_OTYPE_PP;         /* 推挽输出 */
    gpio_init_struct.pupd = GPIO_PUPD_DOWN;         /* 下拉 */
    GPIO_Config(LED0_GPIO_PORT, &gpio_init_struct); /* 配置LED0引脚 */
    
    /* 配置LED1引脚 */
    gpio_init_struct.pin = LED1_GPIO_PIN;           /* LED1引脚 */
    gpio_init_struct.mode = GPIO_MODE_OUT;          /* 输出模式 */
    gpio_init_struct.speed = GPIO_SPEED_100MHz;     /* 高速 */
    gpio_init_struct.otype = GPIO_OTYPE_PP;         /* 推挽输出 */
    gpio_init_struct.pupd = GPIO_PUPD_DOWN;         /* 下拉 */
    GPIO_Config(LED1_GPIO_PORT, &gpio_init_struct); /* 配置LED1引脚 */
    
    /* 默认关闭所有LED */
    LED0(1);
    LED1(1);
}

