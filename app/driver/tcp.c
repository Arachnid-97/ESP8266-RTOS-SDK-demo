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

#include "tcp.h"

#include "bsp_tcp.h"

LOCAL struct espconn esp_conn;
LOCAL esp_tcp esptcp;

/******************************************************************************
 * FunctionName : tcp_server_sent_cb
 * Description  : data sent callback.
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR tcp_sent_cb(void *arg)
{
   //data sent successfully

    os_printf(">>>>> tcp sent succeed !!! \r\n");
    user_TCP_Send();
}


/******************************************************************************
 * FunctionName : tcp_server_recv_cb
 * Description  : receive callback.
 * Parameters   : arg -- Additional argument to pass to the callback function
 *                pusrdata -- The received data (or NULL when the connection has been closed!)
 *                length -- The length of received data
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR tcp_recv_cb(void *arg, char *pusrdata, unsigned short length)
{
   //received some data from tcp connection

   struct espconn *pespconn = arg;
   os_printf(">>>>> tcp recv : %s \r\n", pusrdata);
   user_TCP_Reveive();

//   espconn_send(pespconn, pusrdata, length);

}

/******************************************************************************
 * FunctionName : tcp_server_discon_cb
 * Description  : disconnect callback.
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR tcp_discon_cb(void *arg)
{
   //tcp disconnect successfully
    os_printf(">>>>> tcp disconnect succeed !!! \r\n");
}

/******************************************************************************
 * FunctionName : tcp_server_recon_cb
 * Description  : reconnect callback, error occured in TCP connection.
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR tcp_recon_cb(void *arg, sint8 err)
{
   //error occured , tcp connection broke.

    os_printf(">>>>> reconnect callback, error code %d !!! \r\n",err);
    user_TCP_Abnormal();
}

/******************************************************************************
 * FunctionName : tcp_server_multi_send
 * Description  : TCP server multi send
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
LOCAL void tcp_server_multi_send(void)
{
   struct espconn *pesp_conn = &esp_conn;

   remot_info *premot = NULL;
   uint8 count = 0;
   sint8 value = ESPCONN_OK;

   if (espconn_get_connection_info(pesp_conn,&premot,0) == ESPCONN_OK){
      char *pbuf = "tcp_server_multi_send\n";
      for (count = 0; count < pesp_conn->link_cnt; count ++){
         pesp_conn->proto.tcp->remote_port = premot[count].remote_port;

         pesp_conn->proto.tcp->remote_ip[0] = premot[count].remote_ip[0];
         pesp_conn->proto.tcp->remote_ip[1] = premot[count].remote_ip[1];
         pesp_conn->proto.tcp->remote_ip[2] = premot[count].remote_ip[2];
         pesp_conn->proto.tcp->remote_ip[3] = premot[count].remote_ip[3];

         espconn_sent(pesp_conn, pbuf, os_strlen(pbuf));
      }
   }
}

/******************************************************************************
 * FunctionName : tcp_server_listen_cb
 * Description  : TCP server listened a connection successfully
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR tcp_listen_cb(void *arg)
{
    struct espconn *pesp_conn = arg;
    os_printf(">>>>> tcp_listen !!! \r\n");

    espconn_regist_recvcb(pesp_conn, tcp_recv_cb);		// 设置接收回调
    espconn_regist_sentcb(pesp_conn, tcp_sent_cb);		// 设置发送回调
    espconn_regist_disconcb(pesp_conn, tcp_discon_cb);	// 设置断开连接回调


//   tcp_server_multi_send();// 多服务器发送
}

/******************************************************************************
 * FunctionName : tcp_init
 * Description  : parameter initialize as a TCP server/client
 * Parameters   : mode -- server/client, remote_ip -- remote ip addr, port -- server/client port
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_tcp_init(TCP_MODE_TYPE mode, struct ip_addr *remote_ip, uint32 port)
{
	struct ip_info info;
	esp_conn.proto.tcp = (esp_tcp *) os_zalloc(sizeof(esp_tcp));	// 分配空间

    esp_conn.type = ESPCONN_TCP;			// 创建TCP
    esp_conn.state = ESPCONN_NONE;			// 一开始的状态,空闲状态
    esp_conn.proto.tcp = &esptcp;			// 设置TCP的IP和回调函数存储用
    esp_conn.proto.tcp->local_port = port;	// 监听的端口号

    /* 读取 station IP信息 */
    wifi_get_ip_info(STATION_IF,&info);
    esp_conn.proto.tcp->local_ip[0] = info.ip.addr;
    esp_conn.proto.tcp->local_ip[1] = info.ip.addr >> 8;
    esp_conn.proto.tcp->local_ip[2] = info.ip.addr >> 16;
    esp_conn.proto.tcp->local_ip[3] = info.ip.addr >> 24;
    os_printf("\n>>>>> TCP Local IP: %d.%d.%d.%d\n\n", esp_conn.proto.tcp->local_ip[0], \
    		esp_conn.proto.tcp->local_ip[1], esp_conn.proto.tcp->local_ip[2], \
				esp_conn.proto.tcp->local_ip[3]);

    if(ESPCONN_TCP_CLIENT == mode){
    	memcpy(esp_conn.proto.tcp->remote_ip, remote_ip, 4);
    	esp_conn.proto.tcp->remote_port = port;			// 远程的端口号
    	esp_conn.proto.tcp->local_port += 1;			// 监听的端口号
    }

    espconn_regist_connectcb(&esp_conn, tcp_listen_cb);	// 注册 TCP 连接成功建立后的回调函数
    espconn_regist_reconcb(&esp_conn, tcp_recon_cb);	// 注册 TCP 连接发生异常断开时的回调函数，可以在回调函数中进行重连

    if(ESPCONN_TCP_SERVER == mode){
		espconn_regist_time(&esp_conn, 180, 0); 		// 设置超时断开时间	单位：秒，最大值：7200 秒

		espconn_accept(&esp_conn);						// 创建 TCP server，建立侦听

		os_printf("\n>>>>> tcp server setup successful\n");
    }else if(ESPCONN_TCP_CLIENT == mode){
    	espconn_connect(&esp_conn);						// 创建 TCP client，启用连接

    	os_printf("\n>>>>> tcp client setup successful\n");
    }
}

