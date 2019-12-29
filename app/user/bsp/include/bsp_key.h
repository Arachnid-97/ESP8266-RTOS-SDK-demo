/*
 * bsp_key.h
 *
 *  Created on: 2019年9月2日
 *      Author: liziyuan
 */

#ifndef __BSP_KEY_H
#define __BSP_KEY_H


#include "../../../include/user_config.h"
#include "../../../include/key.h"

#define USER_KEY_NUM		1		// 按键数

#define USER_KEY1_GPIO_ID  			4
#define USER_KEY1_GPIO_FUN 			FUNC_GPIO4
#define USER_KEY1_GPIO_PER			PERIPHS_IO_MUX_GPIO4_U

void ICACHE_FLASH_ATTR user_Key_Init(void);


#endif /* __BSP_KEY_H */


/*------------------------------- END OF FILE -------------------------------*/


