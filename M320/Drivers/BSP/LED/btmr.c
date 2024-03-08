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

#include "includes.h"

/* 外部中断服务子程序 */
volatile uint8_t 	Update_RTC_Time 	= RESET;			// 允许更新RTC时间
volatile uint8_t 	GET_RTC_Time 	 	= RESET;			// 允许获取RTC时间
volatile uint32_t 	time_s 				= TIME_INVALIDS+1;	// 用于计算GPS更新RTC时间以后的秒数
volatile uint32_t   time6_ms = 0;				            // 定时器6时钟计数器，跨文件变量
volatile uint32_t   workLed_ms = 0;				            // 工作指示灯（用于定时器6中断函数中计数）

/* 定时器时间参数 */
#define T6PSC 1000 -1 	//定时器6的PSC值
#define T6ARR 84 -1		//定时器的ARR值

/**
 * @brief       初始化基本定时器中断
 * @note        当APB1PSC!=1时，定时器的时钟频率为APB1时钟的2倍
 *              因此定时器的时钟频率为84MHz
 *              定时器溢出时间计算方法：Tout = ((arr + 1) * (psc + 1)) / TMRxCLK
 *              TMRxCLK=定时器时钟频率，单位MHz
 *              arr * psc = 84
 * 
 *              定时为1ms
 * @param       arr: 自动重装载值
 * @param       psc: 预分频器数值
 * @retval      无
 */
void TIMERx_Init(TMR_T* TIMERx,uint16_t arr, uint16_t psc)
{
    TMR_BaseConfig_T tmr_init_struct;
    
    if(TIMERx == TMR6)
    {
        /* 使能时钟 */
        BTMR_TMRX_INT_CLK_ENABLE();                             /* 使能基本定时器时钟 */

        tmr_init_struct.period = arr;                           /* 自动重装载值 */
        tmr_init_struct.division = psc;                         /* 预分频器数值 */
 
        NVIC_EnableIRQRequest(BTMR_TMRX_INT_IRQn, 1, 0);        /* 开启中断 */
    }
      
    /* 配置基本定时器 */
    TMR_ConfigTimeBase(TIMERx, &tmr_init_struct);    /* 配置基本定时器 */   

    /* 使能基本定时器及其相关中断 */
    TMR_EnableInterrupt(TIMERx, TMR_INT_UPDATE);     /* 使能更新中断 */

    TMR_Enable(TIMERx);                              /* 使能基本定时器 */
}

/**
 * @brief Timer6 Initial
 * @note  Tout = ((arr + 1) * ( psc + 1 )) / Tclk
 * @param psc = 1000 - 1 
 * @param arr = 84 -1
 * @note interval 1ms 
*/
void InitTimers(void)
{
    TIMERx_Init(TMR6,T6ARR,T6PSC);
}

/**
 * @brief       基本定时器中断服务函数
 * @param       时间间隔1ms
 * @retval      无
 */
void BTMR_TMRX_INT_IRQHandler(void)                           // 定时器6，将GPS的数据0.9S发送一次给PC端和LED的0.5S闪烁
{
    if (TMR_ReadIntFlag(BTMR_TMRX_INT, TMR_INT_UPDATE) == SET)// 判断更新中断标志 
    {
		time6_ms++;											  // time6_ms是当前时间的ms总数														
		workLed_ms++;

        if(workLed_ms == 500)
        {
            workLed_ms = 0;
            LED0_TOGGLE();
        }

		if( (time6_ms % 1000) == 900 )						  // 每到900ms的时候,发送数据到PC
		{		
			Time2SendGPSData = SET;							  // 发送数据到PC
            
		}
		
		if(time6_ms >= 86400000)							  // 一天的时间为86400000ms
		{		
			time6_ms = 0;
		}
		
		if(GPS_flags.GPSpps_flag == 1)				         // GPS秒脉冲间隔ms计数器
		{
            GPS_flags.GPSpps_cnt_current++;			
			if(GPS_flags.GPSpps_cnt_current > 86400000)	     // 若间隔超过1天的毫秒数，标志和计数清零
			{
				GPS_flags.GPSpps_flag = 0;
				GPS_flags.GPSpps_cnt_current = 0;
			}
		}                                      
        TMR_ClearIntFlag(BTMR_TMRX_INT, TMR_INT_UPDATE);     // 清除更新中断标志 
    }
}


