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

#include "udp.h"

#include "bsp_udp.h"

LOCAL struct espconn udp_espconn;
LOCAL esp_udp espudp;

/******************************************************************************
 * FunctionName : udp_recv_cb
 * Description  : Processing the received udp packet
 * Parameters   : arg -- Additional argument to pass to the callback function
 *                pusrdata -- The received data (or NULL when the connection has been closed!)
 *                length -- The length of received data
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR udp_recv_cb(void *arg, char *pusrdata, unsigned short length)
{

    os_printf(">>>>> recv udp data: %s\n", pusrdata);
    user_UDP_Reveive();

}

/******************************************************************************
 * FunctionName : udp_send
 * Description  : udp send data
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR udp_send(void)
{
    char DeviceBuffer[40] = "I'm ESP8266";
    char hwaddr[6];
    struct ip_info ipconfig;

    const char udp_remote_ip[4] = { 255, 255, 255, 255};
    os_memcpy(udp_espconn.proto.udp->remote_ip, udp_remote_ip, 4); // ESP8266 udp remote IP need to be set everytime we call espconn_sent
    udp_espconn.proto.udp->remote_port = 1112;  // ESP8266 udp remote port need to be set everytime we call espconn_sent

    wifi_get_macaddr(STATION_IF, hwaddr);

    os_printf("%s" MACSTR "!" , DeviceBuffer, MAC2STR(hwaddr));

    espconn_sent(&udp_espconn, DeviceBuffer, os_strlen(DeviceBuffer));

}

/******************************************************************************
 * FunctionName : udp_sent_cb
 * Description  : udp sent successfully
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
 LOCAL void ICACHE_FLASH_ATTR  udp_sent_cb(void *arg)
 {
	 LOCAL os_timer_t test_timer;

     struct espconn *pespconn = arg;

     os_printf(">>>>> udp send successfully !!!\n");
     user_UDP_Send();

/*
     //disarm timer first
      os_timer_disarm(&test_timer);

     //re-arm timer to check ip
     os_timer_setfn(&test_timer, (os_timer_func_t *)udp_send, NULL); // only send next packet after prev packet sent successfully
     os_timer_arm(&test_timer, 1000, 0);
*/

 }

/******************************************************************************
 * FunctionName : tcp_init
 * Description  : parameter initialize as a udp
 * Parameters   : port -- local port
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_udp_init(uint32 port)
{
	uint8 macaddr[6];

	/* 定义 IP结构体 */
	struct ip_info info;
	udp_espconn.proto.udp = (esp_udp *) os_zalloc(sizeof(esp_udp));	// 分配空间

    udp_espconn.type = ESPCONN_UDP;
    udp_espconn.proto.udp = &espudp;

    /* 读取 station IP信息 */
	wifi_get_ip_info(STATION_IF,&info);

	/* 获取本地 IP地址 */
	udp_espconn.proto.udp->local_ip[0] = info.ip.addr;
	udp_espconn.proto.udp->local_ip[1] = info.ip.addr>>8;
	udp_espconn.proto.udp->local_ip[2] = info.ip.addr>>16;
	udp_espconn.proto.udp->local_ip[3] = info.ip.addr>>24;
	os_printf("\n\n*******************************\n");
	os_printf("UDP Local IP: %d.%d.%d.%d\n", udp_espconn.proto.udp->local_ip[0], \
			udp_espconn.proto.udp->local_ip[1], udp_espconn.proto.udp->local_ip[2], \
				udp_espconn.proto.udp->local_ip[3]);

	/* 端口号  */
	udp_espconn.proto.udp->local_port = port;
	os_printf("UDP Local Port: %d\n", udp_espconn.proto.udp->local_port);

	/* 读取 station MAC */
	wifi_get_macaddr(STATION_IF, macaddr);
	os_printf("Local MAC: %02x.%02x.%02x.%02x.%02x.%02x\n", macaddr[0], macaddr[1], \
			macaddr[2], macaddr[3], macaddr[4], macaddr[5]);

	os_printf("*******************************\n");


    espconn_regist_recvcb(&udp_espconn, udp_recv_cb);		// 接收回调
    espconn_regist_sentcb(&udp_espconn, udp_sent_cb);		// 发送回调

    espconn_create(&udp_espconn);							// 建立 UDP连接
    os_printf("\n>>>>> udp setup successful\n");
}
