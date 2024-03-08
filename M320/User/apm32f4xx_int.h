/*!
 * @file        apm32f4xx_int.h
 *
 * @brief       This file contains the headers of the interrupt handlers
 *
 * @version     V1.0.2
 *
 * @date        2022-06-23
 *
 * @attention
 *
 *  Copyright (C) 2021-2022 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be usefull and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

/* Define to prevent recursive inclusion */
#ifndef __APM32F4XX_INT_H
#define __APM32F4XX_INT_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes */
#include "apm32f4xx.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup Template
  @{
  */

/** @defgroup Template_INT_Functions INT_Functions
  @{
  */

// #define GPS_BUF_SIZE 64
	 
// extern volatile uint32_t time_s;
// extern volatile uint32_t TimingDelay;

// extern volatile uint8_t  gps_index;
// extern volatile uint8_t  GPS_Buf[GPS_BUF_SIZE];
	 
// extern volatile	uint8_t  Update_RTC_Time;
// extern volatile uint8_t  GET_RTC_Time;
// extern volatile uint8_t  Time2SendGPSData;
// extern volatile uint32_t time6_ms;

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void USART1_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /*__APM32F4XX_INT_H */

/**@} end of group Template_INT_Functions */
/**@} end of group Template */
/**@} end of group Examples */
