/*
 * bsp_tcp.c
 *
 *  Created on: 2019年9月4日
 *      Author: liziyuan
 */

#include "esp_common.h"

#include "bsp_tcp.h"



struct espconn user_tcp_conn;
esp_tcp user_tcp;
ip_addr_t user_server_ip;

/******************************************************************************
 * FunctionName : user_TCP_Send
 * Description  : tcp发送回调用户处理
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_TCP_Send(struct espconn *espconn)
{

}

/******************************************************************************
 * FunctionName : user_TCP_Reveive
 * Description  : tcp接收回调用户处理
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_TCP_Reveive(struct espconn *espconn, char *pusrdata, unsigned short length)
{

}

/******************************************************************************
 * FunctionName : user_TCP_Disconnect
 * Description  : tcp断开回调用户处理
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_TCP_Disconnect(struct espconn *pesp_conn)
{

}

/******************************************************************************
 * FunctionName : user_TCP_Abnormal
 * Description  : tcp异常回调用户处理
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_TCP_Abnormal(struct espconn *pesp_conn, sint8 err)
{

}

/******************************************************************************
 * FunctionName : user_TCP_Client_Init
 * Description  : tcp client初始化
 * Parameters   : arg -- the user tcp client network connection structure
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_TCP_Client_Init(struct espconn *arg)
{
	uint8_t remote_ip[4] = {192, 168, 2, 102};
	user_tcp_conn.proto.tcp = (esp_tcp *) os_zalloc(sizeof(esp_tcp));

	memcpy(user_tcp_conn.proto.tcp->remote_ip, &remote_ip, 4);
	user_tcp_conn.proto.tcp->remote_port = USER_TCP_CLIENT_PORT;

	user_tcp_init(ESPCONN_TCP_CLIENT, &user_tcp_conn);
}

/******************************************************************************
 * FunctionName : user_TCP_Server_Init
 * Description  : tcp server初始化
 * Parameters   : arg -- the user tcp server network connection structure
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_TCP_Server_Init(struct espconn *arg)
{
//	uint8_t local_ip[4] = {192, 168, 2, 105};
	user_tcp_conn.proto.tcp = (esp_tcp *) os_zalloc(sizeof(esp_tcp));

//	memcpy(user_tcp_conn.proto.tcp->local_ip, &local_ip, 4);
	user_tcp_conn.proto.tcp->local_port = USER_TCP_SERVER_PORT;

	user_tcp_init(ESPCONN_TCP_SERVER, &user_tcp_conn);
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
	user_TCP_Server_Init(pvParameters);

#else
	user_TCP_Client_Init(pvParameters);

#endif

	vTaskDelete(NULL);
}


/*------------------------------- END OF FILE -------------------------------*/


