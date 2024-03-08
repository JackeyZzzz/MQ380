/**
 ****************************************************************************************************
 * @file        sys.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-15
 * @brief       系统初始化代码（包括时钟配置、中断管理等）
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

#include "./SYSTEM/sys/sys.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_pmu.h"
#include "apm32f4xx_fmc.h"

/**
 * @brief       设置中断向量表偏移地址
 * @param       baseaddr: 基地址
 * @param       offset  : 偏移量
 * @retval      无
 */
void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset)
{
    NVIC_ConfigVectorTable((NVIC_VECT_TAB_T)baseaddr, offset);
}

/**
 * @brief       执行WFI指令(执行完该指令进入睡眠模式, 等待中断唤醒)
 * @param       无
 * @retval      无
 */
void sys_wfi_set(void)
{
    __ASM volatile("wfi");
}

/**
 * @brief       禁用除了NMI和Hard Fault之外的所有异常
 * @param       无
 * @retval      无
 */
void sys_intx_disable(void)
{
    __ASM volatile("cpsid i");
}

/**
 * @brief       启用所有异常
 * @param       无
 * @retval      无
 */
void sys_intx_enable(void)
{
    __ASM volatile("cpsie i");
}

/**
 * @brief       设置栈顶地址
 * @param       addr: 栈顶地址
 * @retval      无
 */
void sys_msr_msp(uint32_t addr)
{
    __set_MSP(addr);
}

/**
 * @brief       进入待机模式
 * @param       无
 * @retval      无
 */
void sys_standby(void)
{
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);
    PMU_EnterSTANDBYMode();
}

/**
 * @brief       系统软复位
 * @param       无
 * @retval      无
 */
void sys_soft_reset(void)
{
    NVIC_SystemReset();
}

/**
 * @brief       配置系统时钟
 * @param       pll1a: PLL1倍频系数（PLL倍频），取值范围：50~432
 * @param       pllb : PLL1预分频系数（进PLL之前的分频），取值范围：2~63
 * @param       pll1c: PLL1的1C分频系数（PLL之后的分频），分频后作为系统时钟，取值范围：2、4、6、8（仅限这4个值！）
 * @param       plld : PLL1的D分频系数（PLL之后的分频），取值范围：2~15
 * @note        f(VCO clock)                   : VCO频率
 *              f(PLL1 clock input)            : 输入PLL1的时钟频率
 *              f(PLL1 clock output)           : PLL1输出的时钟频率
 *              f(OTG_FS,SDIO,RNG clock output): PLL1输出给OTG_FS、SDIO、RNG的时钟频率
 *              f(VCO clock) = f(PLL1 clock input) * (PLL1A / PLLB)
 *              f(PLL1 clock output) = f(VCO clock) / PLL1C
 *              f(OTG_FS,SDIO,RNG clock output) = f(VCO clock) / PLLD
 *              外部晶振频率为8MHz的时候，推荐值：pll1a = 336，pllb = 8，pll1c = 2，plld = 7。
 *              f(VCO clock) = 8 * (336 / 8) = 336MHz
 *              f(PLL1 clock output) = 336 / 2 = 168MHz
 *              f(OTG_FS,SDIO,RNG clock output) = 336 / 7 = 48MHz
 * @retval      0: 成功
 *              1: 失败
 */
uint8_t sys_apm32_clock_init(uint32_t pll1a, uint32_t pllb, uint32_t pll1c, uint32_t plld)
{
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);                                             /* 使能PMU时钟 */
    PMU_ConfigMainRegulatorMode(PMU_REGULATOR_VOLTAGE_SCALE1);                                  /* 设置调节器输出电压级别 */
    
    RCM_Reset();                                                                                /* 复位RCM */
    RCM_ConfigHSE(RCM_HSE_OPEN);                                                                /* 使能外部高速时钟 */
    if (RCM_WaitHSEReady() != SUCCESS)                                                          /* 等待HSECLK稳定 */
    {
        return 1;
    }
    
    FMC_EnablePrefetchBuffer();                                                                 /* 使能Flash预取缓冲区 */
    FMC_ConfigLatency(FMC_LTNCY_5);                                                             /* 设置Flash访问等待周期个数为5个 */
    
    RCM_ConfigAHB(RCM_AHB_DIV_1);                                                               /* 设置AHB时钟的预分频系数为1 */
    RCM_ConfigAPB2(RCM_APB_DIV_2);                                                              /* 设置APB2时钟的预分频系数为2 */
    RCM_ConfigAPB1(RCM_APB_DIV_4);                                                              /* 设置APB1时钟的预分频系数为4 */
    
    RCM_ConfigPLL1(RCM_PLLSEL_HSE, pllb, pll1a, (RCM_PLL_SYS_DIV_T)((pll1c >> 1) - 1), plld);   /* 配置PLL1 */
    RCM_EnablePLL1();                                                                           /* 使能PLL1 */
    while (RCM_ReadStatusFlag(RCM_FLAG_PLL1RDY) == RESET);                                      /* 等待PLL1时钟就绪 */
    
    RCM_ConfigSYSCLK(RCM_SYSCLK_SEL_PLL);                                                       /* 选择PLL1CLK作为系统时钟 */
    while (RCM_ReadSYSCLKSource() != RCM_SYSCLK_SEL_PLL);                                       /* 等待系统时钟配置成功 */
    
    return 0;
}
