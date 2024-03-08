/**
 * @file int.c
 * @author JackeyZ
 * @brief (1) 功能:		用于处理中断相关的数据
 * @version 0.1
 * @date 2023-12-28
 * 
 * @copyright Copyright (c) 2023 SEEKWAY
 * 
 */

#include "includes.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup Template
  @{
  */

/** @defgroup Template_INT_Functions INT_Functions
  @{
  */

/*!
 * @brief   This function handles NMI exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void NMI_Handler(void)
{
}

/*!
 * @brief   This function handles Hard Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void HardFault_Handler(void)
{
    /** Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Memory Manage exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void MemManage_Handler(void)
{
    /** Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Bus Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void BusFault_Handler(void)
{
    /** Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Usage Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void UsageFault_Handler(void)
{
    /** Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles SVCall exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void SVC_Handler(void)
{
}

/*!
 * @brief   This function handles Debug Monitor exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void DebugMon_Handler(void)
{
}

/*!
 * @brief   This function handles PendSV_Handler exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void PendSV_Handler(void)
{
}

/*!
 * @brief   This function handles SysTick Handler
 *
 * @param   None
 *
 * @retval  None
 *
 */
void SysTick_Handler(void)
{
}

/**@} end of group Template_INT_Functions */
/**@} end of group Template */
/**@} end of group Examples */

// //串口2中断服务子程序，用于接收GPS数据	
// volatile uint8_t Time2SendGPSData = RESET;					// 发送GPS数据到PC
// volatile uint8_t gps_index = 0;								// GPS缓冲区“计数器”
// volatile uint8_t GPS_Buf[GPS_BUF_SIZE] = {0};				// GPS缓冲区


// uint8_t RS485_RX_BUF[64] = {0};  	                        // 接收缓冲,最大64个字节.

// /**
//  * @brief       串口1中断服务函数
//  * @param       无
//  * @retval      无
//  */

// void USART1_IRQHandler(void)                                 // 串口一调试用
// {
//     if(USART_ReadStatusFlag(USART1,USART_FLAG_OVRE) != RESET)
//     {
//         USART_ClearStatusFlag(USART1,USART_FLAG_OVRE);
//         USART_RxData(USART1);
//     }
//     if(USART_ReadStatusFlag(USART1,USART_FLAG_RXBNE) == SET)
//     {
//         USART_ClearIntFlag(USART1,USART_INT_RXBNE);

//     }  
// }

// /**
//  * @brief       串口4中断服务函数
//  * @param       无
//  * @retval      无
//  */

// void UART4_IRQHandler(void)                                  // GPS串口使用
// {
// 	if(USART_ReadStatusFlag(UART4,USART_FLAG_OVRE) != RESET)
// 	{
// 		USART_ClearStatusFlag(UART4,USART_FLAG_OVRE);
// 		USART_RxData(UART4);
// 	}
// 	if(USART_ReadStatusFlag(UART4,USART_FLAG_RXBNE) == SET)
// 	{
// 		USART_ClearIntFlag(UART4,USART_INT_RXBNE);
// 		GPS_Buf[gps_index] = USART_RxData(UART4);
// 		gps_index++;
// 		if(gps_index>=GPS_BUF_SIZE)
// 		{
// 			gps_index = 0;
// 		}
// 	}  
// }

// /**
//  * @brief       基本定时器中断服务函数
//  * @param       时间间隔1ms
//  * @retval      无
//  */
// void BTMR_TMRX_INT_IRQHandler(void)                           // 定时器6，将GPS的数据0.9S发送一次给PC端和LED的0.5S闪烁
// {
//     if (TMR_ReadIntFlag(BTMR_TMRX_INT, TMR_INT_UPDATE) == SET)// 判断更新中断标志 
//     {
// 		time6_ms++;											  // time6_ms是当前时间的ms总数														
// 		workLed_ms++;

//         if(workLed_ms == 500)
//         {
//             workLed_ms = 0;
//             LED0_TOGGLE();
//         }

// 		if( (time6_ms % 1000) == 900 )						  // 每到900ms的时候,发送数据到PC
// 		{		
// 			Time2SendGPSData = SET;							  // 发送数据到PC
            
// 		}
		
// 		if(time6_ms >= 86400000)							  // 一天的时间为86400000ms
// 		{		
// 			time6_ms = 0;
// 		}
		
// 		if(GPS_flags.GPSpps_flag == 1)				         // GPS秒脉冲间隔ms计数器
// 		{
//             GPS_flags.GPSpps_cnt_current++;			
// 			if(GPS_flags.GPSpps_cnt_current > 86400000)	     // 若间隔超过1天的毫秒数，标志和计数清零
// 			{
// 				GPS_flags.GPSpps_flag = 0;
// 				GPS_flags.GPSpps_cnt_current = 0;
// 			}
// 		}
		
                                      
//         TMR_ClearIntFlag(BTMR_TMRX_INT, TMR_INT_UPDATE);     // 清除更新中断标志 
//     }
// }


// void EINT15_10_IRQHandler(void)                             // GPS外部中断（1pps）
// {
// 	if(EINT_ReadIntFlag(EINT_LINE_13) == SET)			    // 判断是否发生了GPS秒脉冲中断
// 	{								
//         if(GPS_flags.GPSpps_flag == 0)					    // 首次捕捉到GPS的秒脉冲中断
//         {
//             GPS_flags.GPSpps_flag = 1;
//             GPS_flags.GPSpps_cnt_current = 0;
//         }
//         else												// 只有当相邻两个秒脉冲在设定范围内才认为有效，允许更新RTC
//         {
//             GPS_flags.GPSpps_interval = GPS_flags.GPSpps_cnt_current;
//             GPS_flags.GPSpps_cnt_current = 0;
//             if( (GPS_flags.GPSpps_interval >= GPS_INTERVAL_LOW) && (GPS_flags.GPSpps_interval <= GPS_INTERVAL_HIGH) )
//             {
//                 Update_RTC_Time = SET;					    // 允许更新RTC时间
//             }
//         }	
// 		EINT_ClearIntFlag(EINT_LINE_13);					// 清除外部中断
// 	}

// }

// void EINT0_IRQHandler(void)                                 // RTC外部中断
// {
// 	if(EINT_ReadIntFlag(EINT_LINE_0) == SET)				// 判断是否发生了RTC方波中断
// 	{
// 		time_s++;											// 计算最近一次更新到现在的秒数
		
// 		GET_RTC_Time = SET;									// 允许获取RTC的时间

// 		EINT_ClearIntFlag(EINT_LINE_0);				        // 清除RTC中断
// 	}
// }



// void USART6_IRQHandler(void)
// {
// 	static uint8_t RxState = 0;
// 	static uint8_t pRxPacket = 0;
//     static uint8_t cnt = 0;
//     if(USART_ReadStatusFlag(USART6,USART_FLAG_OVRE) != RESET)
//     {
//         USART_ClearStatusFlag(USART6,USART_FLAG_OVRE);
//         USART_RxData(USART6);
//     }
//     if(USART_ReadStatusFlag(USART6,USART_FLAG_RXBNE) == SET)
//     {
//         USART_ClearIntFlag(USART6,USART_INT_RXBNE);
// 		uint8_t RxData = USART_RxData(USART6);

// 		if (RxState == 0)
// 		{                     
// 			if (RxData == 0x01)
// 			{
//                 cnt = 0;
//                 RxState = 1;              
// 			}
// 		}
// 		else if (RxState == 1)
// 		{            
//             if(RxData == 0xFF)
//             {          
//                 cnt++;               
//             }
//             else
//             {
//                 RxState = 0;
//             }

//             if(cnt == 19)
//             {
//                 RxState = 2;
//                 pRxPacket = 0;
//             }
// 		}
// 		else if (RxState == 2)
// 		{
//             RS485_RX_BUF[pRxPacket] = RxData;  
//             if(pRxPacket >= 4)
//             {
//                 RxState = 0;
//                 if(RS485_RX_BUF[0] == 0xA5 && RS485_RX_BUF[1] == 0x0A && RS485_RX_BUF[2] == 0x0B && RS485_RX_BUF[3] == 0x0C && RS485_RX_BUF[4] == 0x5A)
//                 {                  
//                     SetDmxRevFinish(TRUE);
                    
//                 }
//             }
//             pRxPacket ++;
//         }
//     }   											 
// } 

