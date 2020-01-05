/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "espressif/espconn.h"
#include "espressif/airkiss.h"

#include "bsp/include/bsp_uart.h"
#include "bsp/include/bsp_led.h"
#include "bsp/include/bsp_key.h"
#include "bsp/include/bsp_tcp.h"
#include "bsp/include/bsp_udp.h"

#define DEVICE_TYPE 		"gh_3d67084e28e4" //wechat public number
#define DEVICE_ID 			"gh_3d67084e28e4_4ec32d01aee7310e" //model ID

#define DEFAULT_LAN_PORT 	12476

LOCAL esp_udp ssdp_udp;
LOCAL struct espconn pssdpudpconn;
LOCAL os_timer_t ssdp_time_serv;

uint8  lan_buf[200];
uint16 lan_buf_len;
uint8  udp_sent_cnt = 0;

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

/*===================================
 *  Smartconfig Airkiss FUNCTIONS   *
====================================*/

const airkiss_config_t akconf =
{
	(airkiss_memset_fn)&memset,
	(airkiss_memcpy_fn)&memcpy,
	(airkiss_memcmp_fn)&memcmp,
	0,
};

/******************************************************************************
 * FunctionName : airkiss_wifilan_time_callback
 * Description  : airkiss定时回调
 * Parameters   : arg
 * 				  pdata
 * 				  len
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR airkiss_wifilan_time_callback(void)
{
	uint16 i;
	airkiss_lan_ret_t ret;

	/* airkiss_wifilan_time_callback运行三十次后停止运行 */
	if ((udp_sent_cnt++) >30) {
		udp_sent_cnt = 0;
		os_timer_disarm(&ssdp_time_serv);//s
		//return;
	}

	/* 设置本地udp发送端口号，IP地址设置成255.255.255.255是进行广播，UDP有单播，多播和广播三种模式 */
	ssdp_udp.remote_port = DEFAULT_LAN_PORT;
	ssdp_udp.remote_ip[0] = 255;
	ssdp_udp.remote_ip[1] = 255;
	ssdp_udp.remote_ip[2] = 255;
	ssdp_udp.remote_ip[3] = 255;
	lan_buf_len = sizeof(lan_buf);
	ret = airkiss_lan_pack(AIRKISS_LAN_SSDP_NOTIFY_CMD,
		DEVICE_TYPE, DEVICE_ID, 0, 0, lan_buf, &lan_buf_len, &akconf);
	if (ret != AIRKISS_LAN_PAKE_READY) {
		os_printf("Pack lan packet error!");
		return;
	}

	ret = espconn_sendto(&pssdpudpconn, lan_buf, lan_buf_len);
	if (ret != 0) {
		os_printf("UDP send error!");
	}
	os_printf("Finish send notify!\n");		// UDP发送完成
}

/******************************************************************************
 * FunctionName : airkiss_wifilan_recv_callback
 * Description  : airkiss无线局域网接收回调
 * Parameters   : arg
 * 				  pdata
 * 				  len
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR airkiss_wifilan_recv_callback(void *arg, char *pdata, unsigned short len)
{
	uint16 i;
	remot_info* pcon_info = NULL;

	airkiss_lan_ret_t ret = airkiss_lan_recv(pdata, len, &akconf);
	airkiss_lan_ret_t packret;

	switch (ret){
	case AIRKISS_LAN_SSDP_REQ:
		espconn_get_connection_info(&pssdpudpconn, &pcon_info, 0);
		os_printf("remote ip: %d.%d.%d.%d \r\n",pcon_info->remote_ip[0],pcon_info->remote_ip[1],
			                                    pcon_info->remote_ip[2],pcon_info->remote_ip[3]);
		os_printf("remote port: %d \r\n",pcon_info->remote_port);

        pssdpudpconn.proto.udp->remote_port = pcon_info->remote_port;
		memcpy(pssdpudpconn.proto.udp->remote_ip,pcon_info->remote_ip,4);
		ssdp_udp.remote_port = DEFAULT_LAN_PORT;

		lan_buf_len = sizeof(lan_buf);
		packret = airkiss_lan_pack(AIRKISS_LAN_SSDP_RESP_CMD,
			DEVICE_TYPE, DEVICE_ID, 0, 0, lan_buf, &lan_buf_len, &akconf);

		if (packret != AIRKISS_LAN_PAKE_READY) {
			os_printf("Pack lan packet error!");
			return;
		}

		os_printf("\r\n\r\n");
		for (i=0; i<lan_buf_len; i++)
			os_printf("%c",lan_buf[i]);
		os_printf("\r\n\r\n");

		packret = espconn_sendto(&pssdpudpconn, lan_buf, lan_buf_len);
		if (packret != 0) {
			os_printf("LAN UDP Send err!");
		}

		break;
	default:
		os_printf("Pack is not ssdq req!%d\r\n",ret);
		break;
	}
}

/******************************************************************************
 * FunctionName : airkiss_start_discover
 * Description  : airkiss内网发现
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR airkiss_start_discover(void)
{
	ssdp_udp.local_port = DEFAULT_LAN_PORT;		// 设置本地端口
	pssdpudpconn.type = ESPCONN_UDP;			// 设置通信方式为 UDP
	pssdpudpconn.proto.udp = &(ssdp_udp);
	espconn_regist_recvcb(&pssdpudpconn, airkiss_wifilan_recv_callback);	// 注册收到数据回调函数
	espconn_create(&pssdpudpconn);				// 创建一个UDP传输

	os_timer_disarm(&ssdp_time_serv);
	os_timer_setfn(&ssdp_time_serv, (os_timer_func_t *)airkiss_wifilan_time_callback, NULL);	//注册airkiss定时回调函数
	os_timer_arm(&ssdp_time_serv, 1000, 1);		// 1s
}

/******************************************************************************
 * FunctionName : smartconfig_done
 * Description  : smartconfig 状态发生改变时，进入该函数
 * Parameters   : status
 * 				  pdata
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR smartconfig_done(sc_status status, void *pdata)
{
    switch(status) {
    	/* 连接未开始，请勿在此阶段开始连接 */
        case SC_STATUS_WAIT:
            printf("SC_STATUS_WAIT\n");
            break;

        /* 发现信道，请在此阶段开启 APP进行配对 */
        case SC_STATUS_FIND_CHANNEL:
            printf("SC_STATUS_FIND_CHANNEL\n");
            break;

        /* 获取到 Wi-Fi名称和密码 */
        case SC_STATUS_GETTING_SSID_PSWD:
            printf("SC_STATUS_GETTING_SSID_PSWD\n");
            sc_type *type = pdata;
            /* 判断类型，在这一步发来的数据 pdata中应该有配置类型 */
            if (*type == SC_TYPE_ESPTOUCH) {
                printf("SC_TYPE:SC_TYPE_ESPTOUCH\n");
            } else {
                printf("SC_TYPE:SC_TYPE_AIRKISS\n");
            }
            break;

        /* 开始连接 Wi-Fi */
        case SC_STATUS_LINK:
            printf("SC_STATUS_LINK\n");
            struct station_config *sta_conf = pdata;

	        wifi_station_set_config(sta_conf);		// 设置配置并保存到 flash
	        wifi_station_disconnect();
	        wifi_station_connect();
            break;

        /* 获取到IP，连接路由完成 */
        case SC_STATUS_LINK_OVER:
            printf("SC_STATUS_LINK_OVER\n");
            /* 如果使用的是 SmartConfig，此时手机会将自己的 IP地址发给ESP8266 */
            if (pdata != NULL) {
				//SC_TYPE_ESPTOUCH
                uint8 phone_ip[4] = {0};

                memcpy(phone_ip, (uint8*)pdata, 4);
                printf("Phone ip: %d.%d.%d.%d\n",phone_ip[0],phone_ip[1],phone_ip[2],phone_ip[3]);
            }
            /* 如果是使用的 Airkiss方式，到这一步还没有完成，还会跟微信进行数据交互，应该告知微信配网完成之类的 */
            else {
            	//SC_TYPE_AIRKISS - support airkiss v2.0
				airkiss_start_discover();	// 开始 Airkiss内网发现
			}
            smartconfig_stop();				// SmartConfig 完成，停止配置
            break;
    }

}

/******************************************************************************
 * FunctionName : smartconfig_task
 * Description  : smartconfig任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR smartconfig_task(void *pvParameters)
{
	wifi_station_disconnect();			// 先关闭连接
	os_printf("\n>>>>>>>>>> enter smartconfig mode\n");

	smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS);	// 设置快连模式类型，必须在 smartconfig_start之前调用
	smartconfig_start(smartconfig_done);

	os_printf("\n>>>>>>>>>> smartconfig distribution network wait.....\n");
	while(1)
	{
		if (wifi_station_get_connect_status() == STATION_GOT_IP)	// 等待连接到 AP
		{
//			GLED_ON();
			break;
		}
		vTaskDelay(200 / portTICK_RATE_MS);
	}

	os_printf("\n>>>>>>>>>> esp8266 distribution network success\n\n");
    vTaskDelete(NULL);
}

/******************************************************************************
 * FunctionName : user_main
 * Description  : 用户函数配置入口
 * Parameters   : pvParameters
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_main(void *pvParameters)
{
	struct station_config wifistation;
	uint16_t time = 600;

	uesr_LED_Init();
	user_Key_Init();

	RLED_ON();
	if(STATION_MODE != wifi_get_opmode())
	{
		wifi_set_opmode_current(STATION_MODE);
	}

	wifi_station_get_config_default(&wifistation);
	os_printf("\n/*===================================\n");
	os_printf("\nflash wifi station ssid:%s\n",&wifistation.ssid);
	os_printf("flash wifi station password:%s\n",&wifistation.password);
	os_printf("\n====================================*/\n");

//	wifi_station_connect();						// 把WiFi station 连接到 AP(路由)
	os_printf("\n>>>>>>>>>> ESP8266 station Connection wait.....\n");

	/* 配网状态 led检测 */
	os_timer_disarm(&WifiLED_250ms);
	os_timer_setfn(&WifiLED_250ms, (os_timer_func_t *)wifiled_state_cb, NULL);
	os_timer_arm(&WifiLED_250ms, 250, 1);

	while (1)
	{
		if (wifi_station_get_connect_status() == STATION_GOT_IP)	// 等待连接到 AP
		{
			os_printf("\n>>>>>>>>>> ESP8266 station got IP address from AP\n");
			break;
		}
		if(0 == time)		// 1分钟超时
		{
			os_printf("\n>>>>>>>>>> ESP8266 distribution network overtime\n");
			xTaskCreate(smartconfig_task, "smartconfig_task", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
			break;
		}

		vTaskDelay(100 / portTICK_RATE_MS);		// 100 ms
		time--;
	}
	os_printf("\n>>>>>>>>>> facility task start run\n");

#if 1

#if 1
	xTaskCreate(tcp_communication_task, "tcp_communication_task", 256, NULL, tskIDLE_PRIORITY + 3, NULL);		// 建立 TCP连接

#else
	xTaskCreate(udp_communication_task, "udp_communication_task", 256, NULL, tskIDLE_PRIORITY + 3, NULL);		// 建立 UDP连接

#endif

#endif


	vTaskDelete(NULL);			// 删除自己
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    espconn_init();		// 如果使用的是RTOS，而非NONOS，若需要用到 espconn函数，请在 user_init中调用此 espconn_init，否则会出现连接断开，再连接再断开

    user_Uart_Init();

    vTaskDelay(200 / portTICK_RATE_MS);
    os_printf("\n\n\n");
    os_printf("\n****************************************\n\n");
    os_printf("SDK version:%s\n", system_get_sdk_version());
    os_printf("ESP8266 chip ID:0x%x\n", system_get_chip_id());
    os_printf("ESP8266 flash size map:%d\n", system_get_flash_size_map());
    os_printf("esp debug heap_size %d\n", system_get_free_heap_size());					// 查询系统剩余可用 heap区空间大小
    os_printf("\n>>>>>>>> userbin: %02d <<<<<<<<\n",system_upgrade_userbin_check());	// 云端升级 user.bin查询
    os_printf("\n****************************************\n\n");

//    wifi_set_opmode_current(NULL_MODE);													// 先关闭 WIFI工作模式(自动输出工作模式信息)，不保存在flash中

    xTaskCreate(user_main, "user_main", 400, NULL, tskIDLE_PRIORITY + 1, NULL);			// 创建 “user_main”任务
}


/*------------------------------- END OF FILE -------------------------------*/


