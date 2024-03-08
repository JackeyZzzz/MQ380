/**
 ****************************************************************************************************
 * @file        delay.c
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

#include "./SYSTEM/delay/delay.h"
#include "apm32f4xx_misc.h"

/* ΢�뼶��ʱ������ */
static uint32_t g_fac_us = 0;

/* ֧��OS��������uC/OS�� */
#if (SYS_SUPPORT_OS != 0)

/* ��ӹ���ͷ�ļ���uC/OS��Ҫ�õ��� */
#include "includes.h"

/* ���뼶��ʱ������ */
static uint16_t g_fac_ms = 0;

/*
 * ��delay_us/delay_ms��Ҫ֧��OS��ʱ����Ҫ������OS��صĺ궨��ͺ�����֧��
 * ������3���궨��:
 *     delay_osrunning    : ���ڱ�ʾOS��ǰ�Ƿ��������У��Ծ����Ƿ����ʹ����غ���
 *     delay_ostickspersec: ���ڱ�ʾOS�趨��ʱ�ӽ��ģ�delay_init�����������������ʼ��systick
 *     delay_osintnesting : ���ڱ�ʾOS�ж�Ƕ�׼�����Ϊ�ж����治���Ե��ȣ�delay_msʹ�øò����������������
 * Ȼ����3������:
 *     delay_osschedlock  : ��������OS������ȣ���ֹ����
 *     delay_osschedunlock: ���ڽ���OS������ȣ����¿�������
 *     delay_ostimedly    : ����OS��ʱ�����������������.
 * �����̽���uC/OS-II��uC/OS-III��֧�֣�����OS�������вο���ֲ
 */
 
/* ֧��UCOSII */
#ifdef  OS_CRITICAL_METHOD                          /* OS_CRITICAL_METHOD�����ˣ�˵��Ҫ֧��uC/OS-II */
#define delay_osrunning     OSRunning               /* OS�Ƿ����б�ǣ�0�������У�1�������� */
#define delay_ostickspersec OS_TICKS_PER_SEC        /* OSʱ�ӽ��ģ���ÿ����ȴ��� */
#define delay_osintnesting  OSIntNesting            /* �ж�Ƕ�׼��𣬼��ж�Ƕ�״��� */
#endif

/* ֧��UCOSIII */
#ifdef  CPU_CFG_CRITICAL_METHOD                     /* CPU_CFG_CRITICAL_METHOD�����ˣ�˵��Ҫ֧��uC/OS-III */
#define delay_osrunning     OSRunning               /* OS�Ƿ����б�ǣ�0,�����У�1�������� */
#define delay_ostickspersec OSCfg_TickRate_Hz       /* OSʱ�ӽ��ģ���ÿ����ȴ��� */
#define delay_osintnesting  OSIntNestingCtr         /* �ж�Ƕ�׼��𣬼��ж�Ƕ�״��� */
#endif

/**
 * @brief       �ر�������ȣ���ֹ���΢�뼶��ʱ��
 * @param       ��
 * @retval      ��
 */  
void delay_osschedlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD                      /* ʹ��uC/OS-III */
    OS_ERR err;
    OSSchedLock(&err);                              /* uC/OS-III�ķ�ʽ����ֹ���� */
#else                                               /* ʹ��uC/OS-II */
    OSSchedLock();                                  /* uC/OS-II�ķ�ʽ����ֹ���� */
#endif
}

/**
 * @brief       �ָ��������
 * @param       ��
 * @retval      ��
 */  
void delay_osschedunlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD                      /* ʹ��uC/OS-III */
    OS_ERR err;
    OSSchedUnlock(&err);                            /* uC/OS-III�ķ�ʽ���ָ����� */
#else                                               /* ʹ��uC/OS-II */
    OSSchedUnlock();                                /* uC/OS-II�ķ�ʽ���ָ����� */
#endif
}

/**
 * @brief       ϵͳ������ʱ
 * @param       ticks: ��ʱ�Ľ�����
 * @retval      ��
 */  
void delay_ostimedly(uint32_t ticks)
{
#ifdef CPU_CFG_CRITICAL_METHOD
    OS_ERR err; 
    OSTimeDly(ticks, OS_OPT_TIME_PERIODIC, &err);   /* uC/OS-III�ķ�ʽ��ϵͳ������ʱ */
#else
    OSTimeDly(ticks);                               /* uC/OS-II�ķ�ʽ��ϵͳ������ʱ */
#endif 
}

/**
 * @brief       SysTick�жϷ�����
 * @param       ��
 * @retval      ��
 */  
void SysTick_Handler(void)
{
    if (delay_osrunning == 1)                       /* OS��ʼ���ˣ���ִ�������ĵ��ȴ��� */
    {
        OSIntEnter();                               /* �����ж� */
        OSTimeTick();                               /* ����uC/OS��ʱ�ӷ������ */
        OSIntExit();                                /* �˳��ж� */
    }
}

#endif

/**
 * @brief       ��ʼ����ʱ��
 * @param       sysclk: ϵͳʱ��Ƶ�ʣ�SysTick��ʱ��Ƶ�ʣ�����λ��MHz
 * @retval      ��
 */
void delay_init(uint16_t sysclk)
{
#if (SYS_SUPPORT_OS != 0)
    uint32_t reload;
#endif
    
    SysTick->CTRL = 0;
    SysTick_ConfigCLKSource(SYSTICK_CLK_SOURCE_HCLK);   /* ����SysTickʱ��Դ */
    g_fac_us = sysclk;                                  /* ��ʱ��λ΢����Ҫ��ʱ�ӽ����� */
#if (SYS_SUPPORT_OS != 0)
    reload = sysclk * (1000000 / delay_ostickspersec);  /* ����delay_ostickspersec�趨�Ľ���Ƶ�ʣ�
                                                           ����SysTick����װ��ֵ��
                                                           ��168MHz��ʱ��Ƶ���£�SysTick���ļ���ֵ����ʱ��Ϊ(2^24-1)/168us��ԼΪ99.864ms
                                                         */
    g_fac_ms = 1000 / delay_ostickspersec;              /* һ��ϵͳʱ�ӽ��ĺ�ʱ������ */ 
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;          /* ����SysTick�ж� */
    SysTick->LOAD = reload - 1;                         /* ÿ1/delay_ostickspersec���ж�һ�� */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;           /* ����SysTick */
#endif
}

#if (SYS_SUPPORT_OS != 0)

/**
 * @brief       ΢�뼶��ʱ
 * @param       nus: ��ʱ��΢��������Χ��0~(2^32)/sysclk
 * @retval      ��
 */
void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t reload;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    
    ticks = nus * g_fac_us;                 /* ������Ҫ��ʱ��SysTick������ */
    reload = SysTick->LOAD;                 /* ��ȡSysTick����װ��ֵ */
    delay_osschedlock();                    /* ��ֹOS������ȣ���������ʱ */
    told = SysTick->VAL;                    /* ��ȡ��ʼ��ʱʱ��SysTick����ֵ */
    
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;        /* ��ע��SysTick��һ���ݼ������� */
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
    
    delay_osschedunlock();                  /* �ָ�OS������� */
}

/**
 * @brief       ���뼶��ʱ
 * @param       nms: ��ʱ�ĺ���������Χ��0~(2^16 - 1)
 * @retval      ��
 */
void delay_ms(uint16_t nms)
{
    if (delay_osrunning && (delay_osintnesting == 0))   /* OS�������Ҳ����ж��У��ж��в��ܽ���������ȣ�Ҳ�Ͳ��ܽ��з�������ʱ�� */
    {
        if (nms >= g_fac_ms)                            /* OS��С��ʱʱ��Ϊ1��ϵͳʱ�ӽ��� */
        { 
            delay_ostimedly(nms / g_fac_ms);            /* OS��ʱ */
        }
        nms %= g_fac_ms;                                /* ʣ��ʱ�������ͨ��ʽ������ʱ */
    }                                        
    delay_us(nms * 1000);                               /* ��ͨ��ʽ��ʱ */
}

#else

/**
 * @brief       ΢�뼶��ʱ
 * @param       nus: ��ʱ��΢��������Χ��0~((2^24)/g_fac_us)
 * @retval      ��
 */
void delay_us(uint32_t nus)
{
    uint32_t temp;
    
    if (nus == 0)                               /* ��ʱʱ��Ϊ0���򲻽�����ʱ */
    {
        return;
    }
    
    SysTick->LOAD = (nus * g_fac_us) - 1;       /* ������Ҫ����ʱʱ������SysTickװ��ֵ */
    SysTick->VAL = 0;                           /* ���SysTick����ֵ */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   /* ����SysTick */
    do {                                        /* �ȴ�ʱ�䵽�� */
        temp = SysTick->CTRL;
    } while (((temp & SysTick_CTRL_ENABLE_Msk) != 0) && ((temp & SysTick_CTRL_COUNTFLAG_Msk) == 0));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  /* �ر�SysTick */
    SysTick->VAL = 0;                           /* ���SysTick����ֵ */
}

/**
 * @brief       ���뼶��ʱ
 * @param       nms: ��ʱ�ĺ���������Χ��0~(2^16 - 1)
 * @retval      ��
 */
void delay_ms(uint16_t nms)
{
    uint32_t repeat;
    uint32_t remain;
    
    repeat = nms / 30;          /* delay_us���������ʱʱ�����ޣ���˷ֶ�ν�����ʱ
                                 * ������30���ǿ��ǵ������г�ƵӦ��
                                 */
    remain = nms % 30;
    
    while (repeat != 0)         /* ��repeat�ν���30000us����ʱ */
    {
        delay_us(30 * 1000);
        repeat--;
    }
    
    if (remain != 0)            /* ��delay_us����ʣ��ʱ�����ʱʱ�� */
    {
        delay_us(remain * 1000);
    }
}

#endif
