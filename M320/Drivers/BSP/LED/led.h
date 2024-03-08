/**
 * @file led.c
 * @author JackeyZ
 * @brief (1) 功能:		LED初始化头文件
 * @version 0.1
 * @date 2023-12-28
 * 
 * @copyright Copyright (c) 2023 SEEKWAY
 * 
 */
#ifndef __LED_H
#define __LED_H

#include "./SYSTEM/sys/sys.h"
#include "apm32f4xx_gpio.h"

/* 引脚定义 */
#define LED0_GPIO_PORT          GPIOC
#define LED0_GPIO_PIN           GPIO_PIN_9
#define LED0_GPIO_CLK_ENABLE()  do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOC); }while(0)

#define LED1_GPIO_PORT          GPIOF
#define LED1_GPIO_PIN           GPIO_PIN_10
#define LED1_GPIO_CLK_ENABLE()  do{ RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOF); }while(0)

/* IO操作 */
#define LED0(x)                 do{ x ?                                             \
                                    GPIO_ResetBit(LED0_GPIO_PORT, LED0_GPIO_PIN) :  \
                                    GPIO_SetBit(LED0_GPIO_PORT, LED0_GPIO_PIN);     \
                                }while(0)

#define LED1(x)                 do{ x ?                                             \
                                    GPIO_SetBit(LED1_GPIO_PORT, LED1_GPIO_PIN) :    \
                                    GPIO_ResetBit(LED1_GPIO_PORT, LED1_GPIO_PIN);   \
                                }while(0)

#define LED0_TOGGLE()           do{ GPIO_ToggleBit(LED0_GPIO_PORT, LED0_GPIO_PIN); }while(0)
#define LED1_TOGGLE()           do{ GPIO_ToggleBit(LED1_GPIO_PORT, LED1_GPIO_PIN); }while(0)

/* 函数声明 */
void InitLed(void);    /* 初始化LED */

#endif
