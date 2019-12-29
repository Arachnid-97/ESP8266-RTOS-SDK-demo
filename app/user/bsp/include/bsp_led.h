/*
 * bsp_key.h
 *
 *  Created on: 2019年8月30日
 *      Author: liziyuan
 */

#ifndef __BSP_LED_H
#define __BSP_LED_H


#include "../../../include/gpio.h"

#define USER_RLED_GPIO			15
#define USER_RLED_GPIO_PIN		GPIO_Pin_15
#define USER_GLED_GPIO			12
#define USER_GLED_GPIO_PIN		GPIO_Pin_12
#define USER_BLED_GPIO			13
#define USER_BLED_GPIO_PIN		GPIO_Pin_13

/* PWM */
#define PWM_0_OUT_IO_MUX 		PERIPHS_IO_MUX_MTDI_U
#define PWM_0_OUT_IO_NUM 		15
#define PWM_0_OUT_IO_FUNC  		FUNC_GPIO15

#define PWM_1_OUT_IO_MUX 		PERIPHS_IO_MUX_MTDO_U
#define PWM_1_OUT_IO_NUM 		12
#define PWM_1_OUT_IO_FUNC  		FUNC_GPIO12

#define PWM_2_OUT_IO_MUX 		PERIPHS_IO_MUX_MTCK_U
#define PWM_2_OUT_IO_NUM 		13
#define PWM_2_OUT_IO_FUNC  		FUNC_GPIO13

extern os_timer_t WifiLED_250ms;

void ICACHE_FLASH_ATTR wifiled_state_cb(void);
void ICACHE_FLASH_ATTR RLED_ON(void);
void ICACHE_FLASH_ATTR GLED_ON(void);
void ICACHE_FLASH_ATTR BLED_ON(void);
void ICACHE_FLASH_ATTR RLED_OFF(void);
void ICACHE_FLASH_ATTR GLED_OFF(void);
void ICACHE_FLASH_ATTR BLED_OFF(void);
void ICACHE_FLASH_ATTR uesr_LED_Init(void);


#endif /* __BSP_LED_H */


/*------------------------------- END OF FILE -------------------------------*/


