/**
 * @file ds3231s.h
 * @author JackeyZ
 * @brief (1) ����:		���ڴ���RTCģ����ص�����
 * @version 0.1
 * @date 2023-12-28
 * 
 * @copyright Copyright (c) 2023 SEEKWAY
 * 
 */
#ifndef _DS3231S_H__
#define _DS3231S_H__

#include "includes.h"

/* I2C������غ��� */
void MyI2C_W_SCL(uint8_t BitValue);           // I2Cʱ�Ӻ���
void MyI2C_W_SDA(uint8_t BitValue);           // I2C���ݺ���
void MyI2C_Start(void);                       // I2C��ʼ����
void MyI2C_Stop(void);                        // I2C��������
void MyI2C_SendByte(uint8_t Byte);            // I2C�������ݺ���
uint8_t MyI2C_ReceiveByte(void);			  // I2C�������ݺ���
void MyI2C_SendAck(uint8_t AckBit);			  // I2C����Ӧ��
uint8_t MyI2C_ReceiveAck(void);				  // I2C����Ӧ��

/* RTC����ʱ���־λ */
extern volatile	uint8_t  Update_RTC_Time;    // �������RTCʱ��
extern volatile uint8_t  GET_RTC_Time;       // �����ȡRTCʱ��

/* DS3231�����ú��� */
void InitRtc(void);                          // RTC��ʼ��
void UpdateTimeTask(void);					 // RTC���º�����������������
void LoadRtcTimeTask(void);					 // ����RTC������������������
uint8_t TimeCheck(void);					 // �Ƚϴ�RTC��ȡ��ʱ����д���GPS��ʱ��

/* ����RTCʱ�����ݽṹ�� */
typedef struct{
	uint8_t Year;                            // ��
	uint8_t Month;						     // ��
	uint8_t Day;							 // ��
	uint8_t Hour;                            // ʱ
	uint8_t Min;							 // ��
	uint8_t Sec;							 // ��
	uint8_t data_valid_flag;				 // 
	uint32_t totalms;
}Rtc_Type;

/* �Զ���������� */
extern Rtc_Type RTCTime;                     // ���ڴ洢��ǰ��RTCʱ������
extern uint8_t rtc_calibrate_flag;           // ��ʾRTC�ѱ�У׼  

#endif //_DS3231S_H__
