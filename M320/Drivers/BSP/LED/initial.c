/**
 * @file initial.c
 * @author JackeyZ
 * @brief QM320初始化函数,主要包含外设：串口、485、GPS、RTC、LED、定时器
 * @version 0.1
 * @date 2023-12-28
 * 
 * @copyright Copyright (c) 2024 SEEKWAY
 * 
 */

#include "includes.h"

/**
 * @brief System Initial
 * @note  初始化函数 主要内容有：
 *        1、串口初始化
 *        2、RS485初始化   
 *        3、RTC初始化
 *        4、LED初始化
 *        5、定时器初始化
*/
void InitSystem(void)
{
    // 系统函数初始化
    NVIC_ConfigPriorityGroup(NVIC_PRIORITY_GROUP_3);    // 设置中断优先级分组为组3 
    sys_apm32_clock_init(336, 8, 2, 7);                 // 配置系统时钟 
    delay_init(168);                                    // 初始化延时功能 
    
    // 自定义外设初始化
    InitUsarts();                                       // 串口初始化
    InitRs485s();                                       // 485初始化
    InitGps();                                          // GPS初始化
    InitRtc();                                          // RTC初始化
    InitLed();                                          // LED初始化
    InitTimers();  	                                    // 定时器初始化
}


