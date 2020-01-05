/*
 * bsp_udp.c
 *
 *  Created on: 2019年9月5日
 *      Author: liziyuan
 */

#include "esp_common.h"

#include "bsp_udp.h"


struct espconn udp_espconn;
esp_udp espudp;

/******************************************************************************
 * FunctionName : user_UDP_Send
 * Description  : udp发送回调用户处理
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_UDP_Send(void *arg)
{

}

/******************************************************************************
 * FunctionName : user_UDP_Reveive
 * Description  : udp接收回调用户处理
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_UDP_Reveive(void *arg, char *pusrdata, unsigned short length)
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
	udp_espconn.proto.udp = (esp_udp *) os_zalloc(sizeof(esp_udp));	// 分配空间
	udp_espconn.proto.udp = &espudp;

	os_printf("\n>>>>> UDP communication is being created.....\n");

	while(1)
	{
		if (wifi_station_get_connect_status() == STATION_GOT_IP)	// 等待连接到 AP
		{
			break;
		}
		vTaskDelay(300 / portTICK_RATE_MS);
	}

	udp_espconn.proto.udp->local_port = USER_UDP_PORT;
	user_udp_init(&udp_espconn);

	vTaskDelete(NULL);
}


/*------------------------------- END OF FILE -------------------------------*/


