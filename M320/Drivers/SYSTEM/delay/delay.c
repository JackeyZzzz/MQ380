/**
 ****************************************************************************************************
 * @file        delay.c
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

#include "./SYSTEM/delay/delay.h"
#include "apm32f4xx_misc.h"

/* 微秒级延时倍乘数 */
static uint32_t g_fac_us = 0;

/* 支持OS（不限于uC/OS） */
#if (SYS_SUPPORT_OS != 0)

/* 添加公共头文件（uC/OS需要用到） */
#include "includes.h"

/* 毫秒级延时倍乘数 */
static uint16_t g_fac_ms = 0;

/*
 * 当delay_us/delay_ms需要支持OS的时候需要三个与OS相关的宏定义和函数来支持
 * 首先是3个宏定义:
 *     delay_osrunning    : 用于表示OS当前是否正在运行，以决定是否可以使用相关函数
 *     delay_ostickspersec: 用于表示OS设定的时钟节拍，delay_init将根据这个参数来初始化systick
 *     delay_osintnesting : 用于表示OS中断嵌套级别，因为中断里面不可以调度，delay_ms使用该参数来决定如何运行
 * 然后是3个函数:
 *     delay_osschedlock  : 用于锁定OS任务调度，禁止调度
 *     delay_osschedunlock: 用于解锁OS任务调度，重新开启调度
 *     delay_ostimedly    : 用于OS延时，可以引起任务调度.
 * 本例程仅作uC/OS-II和uC/OS-III的支持，其他OS，请自行参考移植
 */
 
/* 支持UCOSII */
#ifdef  OS_CRITICAL_METHOD                          /* OS_CRITICAL_METHOD定义了，说明要支持uC/OS-II */
#define delay_osrunning     OSRunning               /* OS是否运行标记，0，不运行；1，在运行 */
#define delay_ostickspersec OS_TICKS_PER_SEC        /* OS时钟节拍，即每秒调度次数 */
#define delay_osintnesting  OSIntNesting            /* 中断嵌套级别，即中断嵌套次数 */
#endif

/* 支持UCOSIII */
#ifdef  CPU_CFG_CRITICAL_METHOD                     /* CPU_CFG_CRITICAL_METHOD定义了，说明要支持uC/OS-III */
#define delay_osrunning     OSRunning               /* OS是否运行标记，0,不运行；1，在运行 */
#define delay_ostickspersec OSCfg_TickRate_Hz       /* OS时钟节拍，即每秒调度次数 */
#define delay_osintnesting  OSIntNestingCtr         /* 中断嵌套级别，即中断嵌套次数 */
#endif

/**
 * @brief       关闭任务调度（防止打断微秒级延时）
 * @param       无
 * @retval      无
 */  
void delay_osschedlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD                      /* 使用uC/OS-III */
    OS_ERR err;
    OSSchedLock(&err);                              /* uC/OS-III的方式，禁止调度 */
#else                                               /* 使用uC/OS-II */
    OSSchedLock();                                  /* uC/OS-II的方式，禁止调度 */
#endif
}

/**
 * @brief       恢复任务调度
 * @param       无
 * @retval      无
 */  
void delay_osschedunlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD                      /* 使用uC/OS-III */
    OS_ERR err;
    OSSchedUnlock(&err);                            /* uC/OS-III的方式，恢复调度 */
#else                                               /* 使用uC/OS-II */
    OSSchedUnlock();                                /* uC/OS-II的方式，恢复调度 */
#endif
}

/**
 * @brief       系统节拍延时
 * @param       ticks: 延时的节拍数
 * @retval      无
 */  
void delay_ostimedly(uint32_t ticks)
{
#ifdef CPU_CFG_CRITICAL_METHOD
    OS_ERR err; 
    OSTimeDly(ticks, OS_OPT_TIME_PERIODIC, &err);   /* uC/OS-III的方式，系统节拍延时 */
#else
    OSTimeDly(ticks);                               /* uC/OS-II的方式，系统节拍延时 */
#endif 
}

/**
 * @brief       SysTick中断服务函数
 * @param       无
 * @retval      无
 */  
void SysTick_Handler(void)
{
    if (delay_osrunning == 1)                       /* OS开始跑了，才执行正常的调度处理 */
    {
        OSIntEnter();                               /* 进入中断 */
        OSTimeTick();                               /* 调用uC/OS的时钟服务程序 */
        OSIntExit();                                /* 退出中断 */
    }
}

#endif

/**
 * @brief       初始化延时功
 * @param       sysclk: 系统时钟频率（SysTick的时钟频率），单位：MHz
 * @retval      无
 */
void delay_init(uint16_t sysclk)
{
#if (SYS_SUPPORT_OS != 0)
    uint32_t reload;
#endif
    
    SysTick->CTRL = 0;
    SysTick_ConfigCLKSource(SYSTICK_CLK_SOURCE_HCLK);   /* 配置SysTick时钟源 */
    g_fac_us = sysclk;                                  /* 延时单位微秒需要的时钟节拍数 */
#if (SYS_SUPPORT_OS != 0)
    reload = sysclk * (1000000 / delay_ostickspersec);  /* 根据delay_ostickspersec设定的节拍频率，
                                                           设置SysTick的重装在值，
                                                           在168MHz的时钟频率下，SysTick最大的计数值归零时间为(2^24-1)/168us，约为99.864ms
                                                         */
    g_fac_ms = 1000 / delay_ostickspersec;              /* 一个系统时钟节拍耗时毫秒数 */ 
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;          /* 开启SysTick中断 */
    SysTick->LOAD = reload - 1;                         /* 每1/delay_ostickspersec秒中断一次 */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;           /* 开启SysTick */
#endif
}

#if (SYS_SUPPORT_OS != 0)

/**
 * @brief       微秒级延时
 * @param       nus: 延时的微秒数，范围：0~(2^32)/sysclk
 * @retval      无
 */
void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t reload;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    
    ticks = nus * g_fac_us;                 /* 计算需要延时的SysTick节拍数 */
    reload = SysTick->LOAD;                 /* 获取SysTick的重装载值 */
    delay_osschedlock();                    /* 禁止OS任务调度，避免打断延时 */
    told = SysTick->VAL;                    /* 获取开始延时时的SysTick计数值 */
    
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;        /* 需注意SysTick是一个递减计数器 */
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
    
    delay_osschedunlock();                  /* 恢复OS任务调度 */
}

/**
 * @brief       毫秒级延时
 * @param       nms: 延时的毫秒数，范围：0~(2^16 - 1)
 * @retval      无
 */
void delay_ms(uint16_t nms)
{
    if (delay_osrunning && (delay_osintnesting == 0))   /* OS已运行且不再中断中（中断中不能进行任务调度，也就不能进行非阻塞延时） */
    {
        if (nms >= g_fac_ms)                            /* OS最小延时时间为1个系统时钟节拍 */
        { 
            delay_ostimedly(nms / g_fac_ms);            /* OS延时 */
        }
        nms %= g_fac_ms;                                /* 剩余时间采用普通方式进行延时 */
    }                                        
    delay_us(nms * 1000);                               /* 普通方式延时 */
}

#else

/**
 * @brief       微秒级延时
 * @param       nus: 延时的微秒数，范围：0~((2^24)/g_fac_us)
 * @retval      无
 */
void delay_us(uint32_t nus)
{
    uint32_t temp;
    
    if (nus == 0)                               /* 延时时间为0，则不进行延时 */
    {
        return;
    }
    
    SysTick->LOAD = (nus * g_fac_us) - 1;       /* 根据需要的延时时间设置SysTick装载值 */
    SysTick->VAL = 0;                           /* 清空SysTick计数值 */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   /* 开启SysTick */
    do {                                        /* 等待时间到达 */
        temp = SysTick->CTRL;
    } while (((temp & SysTick_CTRL_ENABLE_Msk) != 0) && ((temp & SysTick_CTRL_COUNTFLAG_Msk) == 0));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  /* 关闭SysTick */
    SysTick->VAL = 0;                           /* 清空SysTick计数值 */
}

/**
 * @brief       毫秒级延时
 * @param       nms: 延时的毫秒数，范围：0~(2^16 - 1)
 * @retval      无
 */
void delay_ms(uint16_t nms)
{
    uint32_t repeat;
    uint32_t remain;
    
    repeat = nms / 30;          /* delay_us单次最大延时时间有限，因此分多次进行延时
                                 * 这里用30，是考虑到可能有超频应用
                                 */
    remain = nms % 30;
    
    while (repeat != 0)         /* 分repeat次进行30000us的延时 */
    {
        delay_us(30 * 1000);
        repeat--;
    }
    
    if (remain != 0)            /* 用delay_us进行剩余时间的延时时间 */
    {
        delay_us(remain * 1000);
    }
}

#endif
