/*
 * bsp_key.c
 *
 *  Created on: 2019年9月2日
 *      Author: liziyuan
 */

#include "esp_common.h"

#include "bsp_key.h"


struct single_key_param *single_key[USER_KEY_NUM];
struct keys_param keys;

/******************************************************************************
 * FunctionName : key_short_press
 * Description  : 短按处理
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR key_short_press(void)
{

}

/******************************************************************************
 * FunctionName : key_long_press
 * Description  : 长按处理
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR key_long_press(void)
{
	os_printf("\n>>>>>>>>>> rientrare smartconfig mode\n");
//	GLED_OFF();
	xTaskCreate(smartconfig_task, "smartconfig_task", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
//	smartconfig_task(NULL);
}

/******************************************************************************
 * FunctionName : user_Key_Init
 * Description  : 按键初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_Key_Init(void)
{
	single_key[0] = key_init_single(USER_KEY1_GPIO_ID, USER_KEY1_GPIO_PER,
			USER_KEY1_GPIO_FUN, key_long_press, key_short_press);

    keys.key_num = USER_KEY_NUM;
    keys.single_key = single_key;
    key_init(&keys);
}


/*------------------------------- END OF FILE -------------------------------*/


