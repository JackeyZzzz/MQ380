#ifndef __INCLUDES_H__
#define __INCLUDES_H__	

/* ����ͷ�ļ� */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/LED/initial.h"
#include "./BSP/LED/btmr.h"
#include "./BSP/LED/rs485.h"
#include "./BSP/LED/gps_data_convert.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_syscfg.h"
#include "apm32f4xx_eint.h"
#include "apm32f4xx_int.h"
#include "apm32f4xx_tmr.h"
#include "apm32f4xx_usart.h"
#include "./BSP/LED/ds3231s.h"

/* C����ͷ�ļ� */
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

/* �Զ���� */
#define TRUE                                  1      // TrueΪ��
#define FALSE                                 0      // False0Ϊ��

#define DMA_ALLDATA_LEN                     128

/* ������ */
#define KEY_RELEASE                           0      // �����ɿ�

/* GPS��غ� */
#define TIME_INVALID	                    (1)	     // ����RTC����Чʱ�䳤��,��λ"��",GPS����������ʱ�䳤�����ɻ�ȡ�����ź�ʱ,��Ϊ��Ҫʹ��RTCʱ��
#define TIME_INVALIDS 	       (TIME_INVALID*1)      // Ϊ��GPS��Чʱ�������л���RTC
#define CALIBRATE_TIMES                       3	     // �������λ����ЧGPS�źţ���ΪRTCУ׼��Ч

/* DMX������غ� */
#define REV_DMX_LEN                        0x19      // DMX���ճ���


/* ����������ָ�� */
#define QUERY_MODE                          0X1      // ��ѯģʽ
#define CHANGE_MODE                        0X21      // ����ģʽ
#define QUERY_SPEED                         0X2      // ��ѯ�ٶ�
#define CHANGE_SPEED                       0X22      // �����ٶ�
#define QUERY_BRIGHTNESS                    0x3      // ��ѯ����
#define CHANGE_BRIGHTNESS                  0x23      // ��������
#define QUERY_LOOP_STATUS                   0X5      // ��ѯѭ��״̬
#define CHANGE_LOOP_STATUS                 0X25      // ����ѭ��״̬
#define SELF_INCREASING_EFFECT             0X26      // Ч������
#define SELF_DEFEATING_EFFECT              0X27      // Ч���Լ�
#define VELOCITY_INCREMENT                 0X28      // �ٶ�����
#define VELOCITY_SELF_DECELERATION         0X29      // �ٶ��Լ�
#define SELF_LUMINANCE                     0X2A      // ��������
#define SELF_DIMMING                       0X2B      // �����Լ�
#define WORKING_MODE_SWITCHING             0X2C      // ����ģʽ�л�
#define LOOP_STATE_SWITCHING               0X2D      // ѭ��״̬�л�
#define SPECIFIED_BRIGHTNESS_INCREMENT     0X2E      // ָ�����Ȼ�������
#define SPECIFIED_LUMINANCE_DECLINE        0X2F      // ָ�����Ȼ����ݼ�

/* ����������ָ�� */
#define QUERY_MODE_S                       0XB1      // ��ѯģʽ������ָ�
#define CHANGE_MODE_S                      0XB1      // ����ģʽ������ָ�
#define QUERY_SPEED_S                      0XB2      // ��ѯ�ٶȣ�����ָ�
#define CHANGE_SPEED_S                     0XB2      // �����ٶȣ�����ָ�
#define QUERY_BRIGHTNESS_S                 0xB3      // ��ѯ���ȣ�����ָ�
#define CHANGE_BRIGHTNESS_S                0xB3      // �������ȣ�����ָ�
#define QUERY_LOOP_STATUS_S                0XB5      // ��ѯѭ��״̬������ָ�
#define CHANGE_LOOP_STATUS_S               0XB5      // ����ѭ��״̬������ָ�
#define SELF_INCREASING_EFFECT_S           0XB1      // Ч������������ָ�
#define SELF_DEFEATING_EFFECT_S            0XB1      // Ч������������ָ�
#define VELOCITY_INCREMENT_S               0XB2      // �ٶ�����������ָ�
#define VELOCITY_SELF_DECELERATION_S       0XB2      // �ٶ��Լ�������ָ�
#define SELF_LUMINANCE_S                   0XB3      // ��������������ָ�
#define SELF_DIMMING_S                     0XB3      // �����Լ�������ָ�
#define WORKING_MODE_SWITCHING_S           0XB1      // ����ģʽ�л�������ָ�
#define LOOP_STATE_SWITCHING_S             0XB5      // ѭ��״̬�л�������ָ�
#define SPECIFIED_BRIGHTNESS_INCREMENT_S   0XBE      // ָ�����Ȼ�������������ָ�
#define SPECIFIED_LUMINANCE_DECLINE_s      0XBF      // ָ�����Ȼ����ݼ�������ָ�

#define DATA_LENGTH_ONE                     0x1      // ����������Ϊ1
#define DATA_LENGTH_TWO                     0x2      // ����������Ϊ2
#define DATA_LENGTH_THREE                   0x3      // ����������Ϊ3

#define PATTERN_MODE                        0x1      // ͼ��ģʽ 
#define SPECTRAL_VOICE_CONTROL_MODE         0X2      // Ƶ������ģʽ
#define SPECTRAL_AUDIO_CONTROL_MODE         0x3      // Ƶ������ģʽ
#define DYNAMIC_VOICE_CONTROL_MODE          0x4      // ��������ģʽ
#define DYNAMIC_AUDIO_CONTROL_MODE          0x5      // ��������ģʽ

#define SINGLE_CYCLE                        0x0      // ����ѭ��
#define MULTIPLE_CYCLES                     0x1      // ���ѭ��
#define RANDOM_CYCLES                       0x2      // ���ѭ��

#define EFFECT_ONE                          0x1      // Ч��һ
#define EFFECT_WHO                          0x2      // Ч����
#define EFFECT_THREE                        0x3      // Ч����
#define EFFECT_FOUF                         0x4      // Ч����
#define EFFECT_FIVE                         0x5      // Ч����
#define EFFECT_SIX                          0x6      // Ч����
#define EFFECT_SEVEN                        0x7      // Ч����
#define EFFECT_EIGHT                        0x8      // Ч����
#define EFFECT_NINE                         0x9      // Ч����
#define EFFECT_TEN                          0xA      // Ч��ʮ

#define SPEED_STATE                         0XA      // ��ʾ��ǰ�ٶȵ�λ��10����֡����100ms��ÿ��10֡

#define LUMINANCE_ONE                       0X0      // ��ɫ
#define LUMINANCE_TWO                       0X1      // 1~6%����
#define LUMINANCE_THREE                     0X2      // 7~12%����
#define LUMINANCE_FOUR                      0X3      // 13~25%����
#define LUMINANCE_FIVE                      0X4      // 26~50%����
#define LUMINANCE_SIX                       0X5      // 51~100%����

#endif //__INCLUDES_H__
