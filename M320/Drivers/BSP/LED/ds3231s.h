/**
 * @file ds3231s.h
 * @author JackeyZ
 * @brief (1) 功能:		用于处理RTC模块相关的数据
 * @version 0.1
 * @date 2023-12-28
 * 
 * @copyright Copyright (c) 2023 SEEKWAY
 * 
 */
#ifndef _DS3231S_H__
#define _DS3231S_H__

#include "includes.h"

/* I2C总线相关函数 */
void MyI2C_W_SCL(uint8_t BitValue);           // I2C时钟函数
void MyI2C_W_SDA(uint8_t BitValue);           // I2C数据函数
void MyI2C_Start(void);                       // I2C开始函数
void MyI2C_Stop(void);                        // I2C结束函数
void MyI2C_SendByte(uint8_t Byte);            // I2C发送数据函数
uint8_t MyI2C_ReceiveByte(void);			  // I2C接收数据函数
void MyI2C_SendAck(uint8_t AckBit);			  // I2C发送应答
uint8_t MyI2C_ReceiveAck(void);				  // I2C接收应答

/* RTC更新时间标志位 */
extern volatile	uint8_t  Update_RTC_Time;    // 允许更新RTC时间
extern volatile uint8_t  GET_RTC_Time;       // 允许获取RTC时间

/* DS3231控制用函数 */
void InitRtc(void);                          // RTC初始化
void UpdateTimeTask(void);					 // RTC更新函数（用于主函数）
void LoadRtcTimeTask(void);					 // 加载RTC函数（用于主函数）
uint8_t TimeCheck(void);					 // 比较从RTC读取的时间与写入的GPS的时间

/* 定义RTC时间数据结构体 */
typedef struct{
	uint8_t Year;                            // 年
	uint8_t Month;						     // 月
	uint8_t Day;							 // 日
	uint8_t Hour;                            // 时
	uint8_t Min;							 // 分
	uint8_t Sec;							 // 秒
	uint8_t data_valid_flag;				 // 
	uint32_t totalms;
}Rtc_Type;

/* 自定义变量区域 */
extern Rtc_Type RTCTime;                     // 用于存储当前的RTC时间日期
extern uint8_t rtc_calibrate_flag;           // 表示RTC已被校准  

#endif //_DS3231S_H__
