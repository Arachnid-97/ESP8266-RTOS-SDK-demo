/*
 * bsp_udp.c
 *
 *  Created on: 2019年9月5日
 *      Author: liziyuan
 */

#include "esp_common.h"

#include "bsp_udp.h"


/******************************************************************************
 * FunctionName : user_UDP_Send
 * Description  : udp发送回调用户处理
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_UDP_Send(void)
{

}

/******************************************************************************
 * FunctionName : user_UDP_Reveive
 * Description  : udp接收回调用户处理
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_UDP_Reveive(void)
{

}

/******************************************************************************
 * FunctionName : udp_communication_task
 * Description  : udp通讯任务
 * Parameters   : pvParameters
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR udp_communication_task(void *pvParameters)
{
	os_printf("\n>>>>> UDP communication is being created.....\n");

	while(1)
	{
		if (wifi_station_get_connect_status() == STATION_GOT_IP)	// 等待连接到 AP
		{
			break;
		}
		vTaskDelay(300 / portTICK_RATE_MS);
	}

	user_udp_init(USER_UDP_PORT);

	vTaskDelete(NULL);
}


/*------------------------------- END OF FILE -------------------------------*/


