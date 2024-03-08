/**
 * @file gps_dataconvert.h
 * @author JackeyZ
 * @brief (1) ����:		���ڴ���GPSģ����ص�����ͷ�ļ�
 * @version 0.1
 * @date 2024-01-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __GPS_DATA_CONVERT_H_
#define __GPS_DATA_CONVERT_H_

#include "apm32f4xx_rcm.h"

/* �Զ������ */
#define GPS_INTERVAL_LOW 990                       // �ж�RTC�Ƿ���Ը��µ���Сʱ��
#define GPS_INTERVAL_HIGH 1010                     // �ж�RTC�Ƿ���Ը��µ����ʱ��

/* �Զ�����������ļ�ʹ�ã� */
volatile extern uint8_t  UTCTIME_AVAILABLE;       // UTC��λ��׼��־λ�����ļ�ʹ�ã�
volatile extern uint8_t  RMCTIME_AVAILABLE;       // RMC��λ��׼��־λ�����ļ�ʹ�ã�
volatile extern uint8_t  GPS_STAR_CNT;            // �������������ļ�ʹ�ã�

/* GPSʱ�����ݽṹ�� */
typedef struct{
	uint8_t year;
	uint8_t mon;
	uint8_t day;
	uint8_t hh;
	uint8_t mm;
	uint8_t ss;
	uint8_t sss;
}GPS_TypeDef;		

/* GPSӦ�ñ�־�ṹ�� */
typedef struct{
	uint32_t GPSpps_cnt_current;	    // �������ms����
	uint32_t GPSpps_interval;			// ���α���⵽�����������
	uint8_t GPSpps_flag;			    // �״������־
	uint8_t gps_convert_finish_flag;	// ����gps����ת����ɱ�־
	uint8_t gga_over_flag;				// GGAЭ�������ɱ�־
	uint8_t rmc_over_flag;				// RMCЭ�������ɱ�־
	uint8_t gps_signal_count;		    // ����ָʾ��ǰGPS�ź��Ƿ�����
}GPS_flags_TypeDef;

/* ǰһ��GPS��ȡ�Ĺؼ����� */
typedef struct{
	uint8_t  UTCTIME_AVAILABLE_pre; 	// ��һ��UTCʱ��	
	uint8_t  RMCTIME_AVAILABLE_pre;		// ��һ��RMCʱ��
	uint8_t  GPS_STAR_CNT_pre;			// ��һ��GPS�ļ���ʱ��
	uint32_t UTCTIME_MS_pre;	        // ��һ��UTCʱ������
}GPS_pre_TypeDef;	
	
/* ����GPSʱ�����ݽṹ��ȫ�ֱ��� */
extern GPS_TypeDef GPS;		            // ����GPS�ṹ�壨GPSʱ������ȫ�ֱ��������ļ�ʹ��
extern GPS_pre_TypeDef GPS_pre_para;    // ����GPS_pre_para�ṹ�壨��һ��GPS�Ĺؼ����������ļ�ʹ��
extern GPS_flags_TypeDef GPS_flags;     // ����GPS_flags�ṹ�壨GPS��ر�־λ�����ļ�ʹ��

/* �Զ���GPS��غ��� */
void GPSDataTask(void);                 // ����GPS��غ�����������������
void InitGps(void);                     // ��ʼ��GPS��غ�����1���ⲿ�ж� 2��GPS��־λ��
void SendGPSData(void);                 // ��GPS�����ݴ��������PC�˲�ˢ��GPS�ؼ�����
void GPS_Convert_Start(void);		    // GPSת����ʼ����
void SendGpsTask(void);					// ����GPS�����ݺ�����������������

#endif //__GPS_DATA_CONVERT_H_

