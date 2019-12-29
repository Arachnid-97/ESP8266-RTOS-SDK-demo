/*
 * bsp_tcp.c
 *
 *  Created on: 2019年9月4日
 *      Author: liziyuan
 */

#include "esp_common.h"

#include "bsp_tcp.h"


/******************************************************************************
 * FunctionName : user_TCP_Abnormal
 * Description  : tcp异常回调用户处理
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_TCP_Abnormal(void)
{

}

/******************************************************************************
 * FunctionName : user_TCP_Send
 * Description  : tcp发送回调用户处理
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_TCP_Send(void)
{

}

/******************************************************************************
 * FunctionName : user_TCP_Reveive
 * Description  : tcp接收回调用户处理
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_TCP_Reveive(void)
{

}

/******************************************************************************
 * FunctionName : user_TCP_Client_Init
 * Description  : tcp client初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_TCP_Client_Init(void)
{
	const uint8 remote_ip[4] = {192, 168, 1, 1};

	user_tcp_init(ESPCONN_TCP_CLIENT, (struct ip_addr *)remote_ip, USER_TCP_CLIENT_PORT);
}

/******************************************************************************
 * FunctionName : user_TCP_Server_Init
 * Description  : tcp server初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_TCP_Server_Init(void)
{
	user_tcp_init(ESPCONN_TCP_SERVER, (struct ip_addr *)NULL, USER_TCP_SERVER_PORT);
}

/******************************************************************************
 * FunctionName : tcp_communication_task
 * Description  : tcp通讯任务
 * Parameters   : pvParameters
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR tcp_communication_task(void *pvParameters)
{
	os_printf("\n>>>>> TCP communication is being created.....\n");

	while(1)
	{
		if (wifi_station_get_connect_status() == STATION_GOT_IP)	// 等待连接到 AP
		{
			break;
		}
		vTaskDelay(300 / portTICK_RATE_MS);
	}

#if 1
	user_TCP_Server_Init();

#else
	user_TCP_Client_Init();

#endif

	vTaskDelete(NULL);
}


/*------------------------------- END OF FILE -------------------------------*/


