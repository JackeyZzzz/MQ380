/**
 * @file main.c
 * @author JackeyZ
 * @brief MQ-320������ ��Ҫ���ܻ�ȡGPSʱ�䲢�Ҹ���RTCʱ�䣬ͨ��485����������ƽ̨�ͽ���ƽ̨���ݣ���ѭ�����ܹ���Ϊ�������
 *        1��GPS���ݴ������� 
 *        2��GPS���ݷ���PC������ 
 *        3��RTC����ʱ������ 
 *        4����ȡRTCʱ������ 
 *        5��ͨ��485���պͷ�����������
 * @version 0.1
 * @date 2024-01-19
 * 
 * @copyright Copyright (c) 2024 SEEKWAY
 * 
 */
#include "includes.h"

int main(void)
{
    InitSystem();                             /* ��ʼ������ */ 
 
    while (1)
    {
        GPSDataTask();		                  /* GPS���ݴ���(���ݽ�������ȡ��Ҫ������) */	

        SendGpsTask();                        /* GPS���ݷ���PC��(900ms��һ������) */      

        UpdateTimeTask();                     /* RTC����ʱ��(��GPS��ʱ�����RTC) */

        LoadRtcTimeTask();                    /* ��ȡRTCʱ�� */

        DmxHandleTask();                      /* DMX������(���տ�̨��������֮��������) */

        /* for test */    
    }
}
