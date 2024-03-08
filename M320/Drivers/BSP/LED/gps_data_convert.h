/**
 * @file gps_dataconvert.h
 * @author JackeyZ
 * @brief (1) 功能:		用于处理GPS模块相关的数据头文件
 * @version 0.1
 * @date 2024-01-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __GPS_DATA_CONVERT_H_
#define __GPS_DATA_CONVERT_H_

#include "apm32f4xx_rcm.h"

/* 自定义变量 */
#define GPS_INTERVAL_LOW 990                       // 判断RTC是否可以更新的最小时间
#define GPS_INTERVAL_HIGH 1010                     // 判断RTC是否可以更新的最大时间

/* 自定义变量（跨文件使用） */
volatile extern uint8_t  UTCTIME_AVAILABLE;       // UTC定位精准标志位（跨文件使用）
volatile extern uint8_t  RMCTIME_AVAILABLE;       // RMC定位精准标志位（跨文件使用）
volatile extern uint8_t  GPS_STAR_CNT;            // 卫星数量（跨文件使用）

/* GPS时间数据结构体 */
typedef struct{
	uint8_t year;
	uint8_t mon;
	uint8_t day;
	uint8_t hh;
	uint8_t mm;
	uint8_t ss;
	uint8_t sss;
}GPS_TypeDef;		

/* GPS应用标志结构体 */
typedef struct{
	uint32_t GPSpps_cnt_current;	    // 秒脉冲后ms计数
	uint32_t GPSpps_interval;			// 两次被检测到的秒脉冲计数
	uint8_t GPSpps_flag;			    // 首次脉冲标志
	uint8_t gps_convert_finish_flag;	// 本秒gps数据转换完成标志
	uint8_t gga_over_flag;				// GGA协议接收完成标志
	uint8_t rmc_over_flag;				// RMC协议接收完成标志
	uint8_t gps_signal_count;		    // 用于指示当前GPS信号是否良好
}GPS_flags_TypeDef;

/* 前一次GPS获取的关键参数 */
typedef struct{
	uint8_t  UTCTIME_AVAILABLE_pre; 	// 上一次UTC时间	
	uint8_t  RMCTIME_AVAILABLE_pre;		// 上一次RMC时间
	uint8_t  GPS_STAR_CNT_pre;			// 上一次GPS的计数时间
	uint32_t UTCTIME_MS_pre;	        // 上一次UTC时间总数
}GPS_pre_TypeDef;	
	
/* 声明GPS时间数据结构体全局变量 */
extern GPS_TypeDef GPS;		            // 定义GPS结构体（GPS时间数据全局变量）跨文件使用
extern GPS_pre_TypeDef GPS_pre_para;    // 定义GPS_pre_para结构体（上一次GPS的关键参数）跨文件使用
extern GPS_flags_TypeDef GPS_flags;     // 定义GPS_flags结构体（GPS相关标志位）跨文件使用

/* 自定义GPS相关函数 */
void GPSDataTask(void);                 // 处理GPS相关函数（用于主函数）
void InitGps(void);                     // 初始化GPS相关函数（1、外部中断 2、GPS标志位）
void SendGPSData(void);                 // 将GPS的数据打包发送至PC端并刷新GPS关键参数
void GPS_Convert_Start(void);		    // GPS转换开始函数
void SendGpsTask(void);					// 发送GPS的数据函数（用于主函数）

#endif //__GPS_DATA_CONVERT_H_

