#ifndef __INCLUDES_H__
#define __INCLUDES_H__	

/* 外设头文件 */
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

/* C语言头文件 */
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

/* 自定义宏 */
#define TRUE                                  1      // True为真
#define FALSE                                 0      // False0为假

#define DMA_ALLDATA_LEN                     128

/* 按键宏 */
#define KEY_RELEASE                           0      // 按键松开

/* GPS相关宏 */
#define TIME_INVALID	                    (1)	     // 定义RTC的无效时间长度,单位"秒",GPS如果超出这个时间长度依旧获取不到信号时,认为需要使用RTC时间
#define TIME_INVALIDS 	       (TIME_INVALID*1)      // 为了GPS无效时，马上切换到RTC
#define CALIBRATE_TIMES                       3	     // 连续三次获得有效GPS信号，认为RTC校准有效

/* DMX接收相关宏 */
#define REV_DMX_LEN                        0x19      // DMX接收长度


/* 控制器接收指令 */
#define QUERY_MODE                          0X1      // 查询模式
#define CHANGE_MODE                        0X21      // 更改模式
#define QUERY_SPEED                         0X2      // 查询速度
#define CHANGE_SPEED                       0X22      // 更改速度
#define QUERY_BRIGHTNESS                    0x3      // 查询亮度
#define CHANGE_BRIGHTNESS                  0x23      // 更改亮度
#define QUERY_LOOP_STATUS                   0X5      // 查询循环状态
#define CHANGE_LOOP_STATUS                 0X25      // 更改循环状态
#define SELF_INCREASING_EFFECT             0X26      // 效果自增
#define SELF_DEFEATING_EFFECT              0X27      // 效果自减
#define VELOCITY_INCREMENT                 0X28      // 速度自增
#define VELOCITY_SELF_DECELERATION         0X29      // 速度自减
#define SELF_LUMINANCE                     0X2A      // 亮度自增
#define SELF_DIMMING                       0X2B      // 亮度自减
#define WORKING_MODE_SWITCHING             0X2C      // 工作模式切换
#define LOOP_STATE_SWITCHING               0X2D      // 循环状态切换
#define SPECIFIED_BRIGHTNESS_INCREMENT     0X2E      // 指定亮度基数递增
#define SPECIFIED_LUMINANCE_DECLINE        0X2F      // 指定亮度基数递减

/* 控制器发送指令 */
#define QUERY_MODE_S                       0XB1      // 查询模式（返回指令）
#define CHANGE_MODE_S                      0XB1      // 更改模式（返回指令）
#define QUERY_SPEED_S                      0XB2      // 查询速度（返回指令）
#define CHANGE_SPEED_S                     0XB2      // 更改速度（返回指令）
#define QUERY_BRIGHTNESS_S                 0xB3      // 查询亮度（返回指令）
#define CHANGE_BRIGHTNESS_S                0xB3      // 更改亮度（返回指令）
#define QUERY_LOOP_STATUS_S                0XB5      // 查询循环状态（返回指令）
#define CHANGE_LOOP_STATUS_S               0XB5      // 更改循环状态（返回指令）
#define SELF_INCREASING_EFFECT_S           0XB1      // 效果自增（返回指令）
#define SELF_DEFEATING_EFFECT_S            0XB1      // 效果减增（返回指令）
#define VELOCITY_INCREMENT_S               0XB2      // 速度自增（返回指令）
#define VELOCITY_SELF_DECELERATION_S       0XB2      // 速度自减（返回指令）
#define SELF_LUMINANCE_S                   0XB3      // 亮度自增（返回指令）
#define SELF_DIMMING_S                     0XB3      // 亮度自减（返回指令）
#define WORKING_MODE_SWITCHING_S           0XB1      // 工作模式切换（返回指令）
#define LOOP_STATE_SWITCHING_S             0XB5      // 循环状态切换（返回指令）
#define SPECIFIED_BRIGHTNESS_INCREMENT_S   0XBE      // 指定亮度基数递增（返回指令）
#define SPECIFIED_LUMINANCE_DECLINE_s      0XBF      // 指定亮度基数递减（返回指令）

#define DATA_LENGTH_ONE                     0x1      // 数据区长度为1
#define DATA_LENGTH_TWO                     0x2      // 数据区长度为2
#define DATA_LENGTH_THREE                   0x3      // 数据区长度为3

#define PATTERN_MODE                        0x1      // 图案模式 
#define SPECTRAL_VOICE_CONTROL_MODE         0X2      // 频谱声控模式
#define SPECTRAL_AUDIO_CONTROL_MODE         0x3      // 频谱音控模式
#define DYNAMIC_VOICE_CONTROL_MODE          0x4      // 动感声控模式
#define DYNAMIC_AUDIO_CONTROL_MODE          0x5      // 动感音控模式

#define SINGLE_CYCLE                        0x0      // 单个循环
#define MULTIPLE_CYCLES                     0x1      // 多个循环
#define RANDOM_CYCLES                       0x2      // 随机循环

#define EFFECT_ONE                          0x1      // 效果一
#define EFFECT_WHO                          0x2      // 效果二
#define EFFECT_THREE                        0x3      // 效果三
#define EFFECT_FOUF                         0x4      // 效果四
#define EFFECT_FIVE                         0x5      // 效果五
#define EFFECT_SIX                          0x6      // 效果六
#define EFFECT_SEVEN                        0x7      // 效果七
#define EFFECT_EIGHT                        0x8      // 效果八
#define EFFECT_NINE                         0x9      // 效果九
#define EFFECT_TEN                          0xA      // 效果十

#define SPEED_STATE                         0XA      // 表示当前速度档位是10，即帧速是100ms，每秒10帧

#define LUMINANCE_ONE                       0X0      // 黑色
#define LUMINANCE_TWO                       0X1      // 1~6%亮度
#define LUMINANCE_THREE                     0X2      // 7~12%亮度
#define LUMINANCE_FOUR                      0X3      // 13~25%亮度
#define LUMINANCE_FIVE                      0X4      // 26~50%亮度
#define LUMINANCE_SIX                       0X5      // 51~100%亮度

#endif //__INCLUDES_H__
