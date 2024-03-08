/**
 * @file main.c
 * @author JackeyZ
 * @brief MQ-320主函数 主要功能获取GPS时间并且更新RTC时间，通过485发送数据至平台和接收平台数据，主循环里总共分为五个任务：
 *        1、GPS数据处理任务 
 *        2、GPS数据发送PC端任务 
 *        3、RTC更新时间任务 
 *        4、获取RTC时间任务 
 *        5、通过485接收和发送数据任务
 * @version 0.1
 * @date 2024-01-19
 * 
 * @copyright Copyright (c) 2024 SEEKWAY
 * 
 */
#include "includes.h"

int main(void)
{
    InitSystem();                             /* 初始化外设 */ 
 
    while (1)
    {
        GPSDataTask();		                  /* GPS数据处理(数据解析，获取需要的数据) */	

        SendGpsTask();                        /* GPS数据发送PC端(900ms发一次数据) */      

        UpdateTimeTask();                     /* RTC更新时间(用GPS的时间更新RTC) */

        LoadRtcTimeTask();                    /* 获取RTC时间 */

        DmxHandleTask();                      /* DMX发数据(接收控台发的数据之后发送数据) */

        /* for test */    
    }
}
