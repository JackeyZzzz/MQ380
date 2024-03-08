/**
 * @file btmr.c
 * @author JackeyZ
 * @brief ��ʱ����س�ʼ������
 * @version 0.1
 * @date 2024-01-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "includes.h"

/* �ⲿ�жϷ����ӳ��� */
volatile uint8_t 	Update_RTC_Time 	= RESET;			// �������RTCʱ��
volatile uint8_t 	GET_RTC_Time 	 	= RESET;			// �����ȡRTCʱ��
volatile uint32_t 	time_s 				= TIME_INVALIDS+1;	// ���ڼ���GPS����RTCʱ���Ժ������
volatile uint32_t   time6_ms = 0;				            // ��ʱ��6ʱ�Ӽ����������ļ�����
volatile uint32_t   workLed_ms = 0;				            // ����ָʾ�ƣ����ڶ�ʱ��6�жϺ����м�����

/* ��ʱ��ʱ����� */
#define T6PSC 1000 -1 	//��ʱ��6��PSCֵ
#define T6ARR 84 -1		//��ʱ����ARRֵ

/**
 * @brief       ��ʼ��������ʱ���ж�
 * @note        ��APB1PSC!=1ʱ����ʱ����ʱ��Ƶ��ΪAPB1ʱ�ӵ�2��
 *              ��˶�ʱ����ʱ��Ƶ��Ϊ84MHz
 *              ��ʱ�����ʱ����㷽����Tout = ((arr + 1) * (psc + 1)) / TMRxCLK
 *              TMRxCLK=��ʱ��ʱ��Ƶ�ʣ���λMHz
 *              arr * psc = 84
 * 
 *              ��ʱΪ1ms
 * @param       arr: �Զ���װ��ֵ
 * @param       psc: Ԥ��Ƶ����ֵ
 * @retval      ��
 */
void TIMERx_Init(TMR_T* TIMERx,uint16_t arr, uint16_t psc)
{
    TMR_BaseConfig_T tmr_init_struct;
    
    if(TIMERx == TMR6)
    {
        /* ʹ��ʱ�� */
        BTMR_TMRX_INT_CLK_ENABLE();                             /* ʹ�ܻ�����ʱ��ʱ�� */

        tmr_init_struct.period = arr;                           /* �Զ���װ��ֵ */
        tmr_init_struct.division = psc;                         /* Ԥ��Ƶ����ֵ */
 
        NVIC_EnableIRQRequest(BTMR_TMRX_INT_IRQn, 1, 0);        /* �����ж� */
    }
      
    /* ���û�����ʱ�� */
    TMR_ConfigTimeBase(TIMERx, &tmr_init_struct);    /* ���û�����ʱ�� */   

    /* ʹ�ܻ�����ʱ����������ж� */
    TMR_EnableInterrupt(TIMERx, TMR_INT_UPDATE);     /* ʹ�ܸ����ж� */

    TMR_Enable(TIMERx);                              /* ʹ�ܻ�����ʱ�� */
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
 * @brief       ������ʱ���жϷ�����
 * @param       ʱ����1ms
 * @retval      ��
 */
void BTMR_TMRX_INT_IRQHandler(void)                           // ��ʱ��6����GPS������0.9S����һ�θ�PC�˺�LED��0.5S��˸
{
    if (TMR_ReadIntFlag(BTMR_TMRX_INT, TMR_INT_UPDATE) == SET)// �жϸ����жϱ�־ 
    {
		time6_ms++;											  // time6_ms�ǵ�ǰʱ���ms����														
		workLed_ms++;

        if(workLed_ms == 500)
        {
            workLed_ms = 0;
            LED0_TOGGLE();
        }

		if( (time6_ms % 1000) == 900 )						  // ÿ��900ms��ʱ��,�������ݵ�PC
		{		
			Time2SendGPSData = SET;							  // �������ݵ�PC
            
		}
		
		if(time6_ms >= 86400000)							  // һ���ʱ��Ϊ86400000ms
		{		
			time6_ms = 0;
		}
		
		if(GPS_flags.GPSpps_flag == 1)				         // GPS��������ms������
		{
            GPS_flags.GPSpps_cnt_current++;			
			if(GPS_flags.GPSpps_cnt_current > 86400000)	     // ���������1��ĺ���������־�ͼ�������
			{
				GPS_flags.GPSpps_flag = 0;
				GPS_flags.GPSpps_cnt_current = 0;
			}
		}                                      
        TMR_ClearIntFlag(BTMR_TMRX_INT, TMR_INT_UPDATE);     // ��������жϱ�־ 
    }
}


