/**
 ****************************************************************************************************
 * @file        sys.c
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

#include "./SYSTEM/sys/sys.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_pmu.h"
#include "apm32f4xx_fmc.h"

/**
 * @brief       �����ж�������ƫ�Ƶ�ַ
 * @param       baseaddr: ����ַ
 * @param       offset  : ƫ����
 * @retval      ��
 */
void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset)
{
    NVIC_ConfigVectorTable((NVIC_VECT_TAB_T)baseaddr, offset);
}

/**
 * @brief       ִ��WFIָ��(ִ�����ָ�����˯��ģʽ, �ȴ��жϻ���)
 * @param       ��
 * @retval      ��
 */
void sys_wfi_set(void)
{
    __ASM volatile("wfi");
}

/**
 * @brief       ���ó���NMI��Hard Fault֮��������쳣
 * @param       ��
 * @retval      ��
 */
void sys_intx_disable(void)
{
    __ASM volatile("cpsid i");
}

/**
 * @brief       ���������쳣
 * @param       ��
 * @retval      ��
 */
void sys_intx_enable(void)
{
    __ASM volatile("cpsie i");
}

/**
 * @brief       ����ջ����ַ
 * @param       addr: ջ����ַ
 * @retval      ��
 */
void sys_msr_msp(uint32_t addr)
{
    __set_MSP(addr);
}

/**
 * @brief       �������ģʽ
 * @param       ��
 * @retval      ��
 */
void sys_standby(void)
{
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);
    PMU_EnterSTANDBYMode();
}

/**
 * @brief       ϵͳ��λ
 * @param       ��
 * @retval      ��
 */
void sys_soft_reset(void)
{
    NVIC_SystemReset();
}

/**
 * @brief       ����ϵͳʱ��
 * @param       pll1a: PLL1��Ƶϵ����PLL��Ƶ����ȡֵ��Χ��50~432
 * @param       pllb : PLL1Ԥ��Ƶϵ������PLL֮ǰ�ķ�Ƶ����ȡֵ��Χ��2~63
 * @param       pll1c: PLL1��1C��Ƶϵ����PLL֮��ķ�Ƶ������Ƶ����Ϊϵͳʱ�ӣ�ȡֵ��Χ��2��4��6��8��������4��ֵ����
 * @param       plld : PLL1��D��Ƶϵ����PLL֮��ķ�Ƶ����ȡֵ��Χ��2~15
 * @note        f(VCO clock)                   : VCOƵ��
 *              f(PLL1 clock input)            : ����PLL1��ʱ��Ƶ��
 *              f(PLL1 clock output)           : PLL1�����ʱ��Ƶ��
 *              f(OTG_FS,SDIO,RNG clock output): PLL1�����OTG_FS��SDIO��RNG��ʱ��Ƶ��
 *              f(VCO clock) = f(PLL1 clock input) * (PLL1A / PLLB)
 *              f(PLL1 clock output) = f(VCO clock) / PLL1C
 *              f(OTG_FS,SDIO,RNG clock output) = f(VCO clock) / PLLD
 *              �ⲿ����Ƶ��Ϊ8MHz��ʱ���Ƽ�ֵ��pll1a = 336��pllb = 8��pll1c = 2��plld = 7��
 *              f(VCO clock) = 8 * (336 / 8) = 336MHz
 *              f(PLL1 clock output) = 336 / 2 = 168MHz
 *              f(OTG_FS,SDIO,RNG clock output) = 336 / 7 = 48MHz
 * @retval      0: �ɹ�
 *              1: ʧ��
 */
uint8_t sys_apm32_clock_init(uint32_t pll1a, uint32_t pllb, uint32_t pll1c, uint32_t plld)
{
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);                                             /* ʹ��PMUʱ�� */
    PMU_ConfigMainRegulatorMode(PMU_REGULATOR_VOLTAGE_SCALE1);                                  /* ���õ����������ѹ���� */
    
    RCM_Reset();                                                                                /* ��λRCM */
    RCM_ConfigHSE(RCM_HSE_OPEN);                                                                /* ʹ���ⲿ����ʱ�� */
    if (RCM_WaitHSEReady() != SUCCESS)                                                          /* �ȴ�HSECLK�ȶ� */
    {
        return 1;
    }
    
    FMC_EnablePrefetchBuffer();                                                                 /* ʹ��FlashԤȡ������ */
    FMC_ConfigLatency(FMC_LTNCY_5);                                                             /* ����Flash���ʵȴ����ڸ���Ϊ5�� */
    
    RCM_ConfigAHB(RCM_AHB_DIV_1);                                                               /* ����AHBʱ�ӵ�Ԥ��Ƶϵ��Ϊ1 */
    RCM_ConfigAPB2(RCM_APB_DIV_2);                                                              /* ����APB2ʱ�ӵ�Ԥ��Ƶϵ��Ϊ2 */
    RCM_ConfigAPB1(RCM_APB_DIV_4);                                                              /* ����APB1ʱ�ӵ�Ԥ��Ƶϵ��Ϊ4 */
    
    RCM_ConfigPLL1(RCM_PLLSEL_HSE, pllb, pll1a, (RCM_PLL_SYS_DIV_T)((pll1c >> 1) - 1), plld);   /* ����PLL1 */
    RCM_EnablePLL1();                                                                           /* ʹ��PLL1 */
    while (RCM_ReadStatusFlag(RCM_FLAG_PLL1RDY) == RESET);                                      /* �ȴ�PLL1ʱ�Ӿ��� */
    
    RCM_ConfigSYSCLK(RCM_SYSCLK_SEL_PLL);                                                       /* ѡ��PLL1CLK��Ϊϵͳʱ�� */
    while (RCM_ReadSYSCLKSource() != RCM_SYSCLK_SEL_PLL);                                       /* �ȴ�ϵͳʱ�����óɹ� */
    
    return 0;
}
