/*
 * bsp_tcp.h
 *
 *  Created on: 2019年9月4日
 *      Author: liziyuan
 */

#ifndef __BSP_TCP_H
#define __BSP_TCP_H


#include "../../../include/tcp.h"

#define USER_TCP_SERVER_PORT		8080
#define USER_TCP_CLIENT_PORT		8000

extern struct espconn user_tcp_conn;
extern esp_tcp user_tcp;
extern ip_addr_t user_server_ip;

void ICACHE_FLASH_ATTR user_TCP_Send(struct espconn *espconn);
void ICACHE_FLASH_ATTR user_TCP_Reveive(struct espconn *espconn, char *pusrdata, unsigned short length);
void ICACHE_FLASH_ATTR user_TCP_Disconnect(struct espconn *pesp_conn);
void ICACHE_FLASH_ATTR user_TCP_Abnormal(struct espconn *pesp_conn, sint8 err);
void ICACHE_FLASH_ATTR user_TCP_Client_Init(struct espconn *arg);
void ICACHE_FLASH_ATTR user_TCP_Server_Init(struct espconn *arg);
void ICACHE_FLASH_ATTR tcp_communication_task(void *pvParameters);


#endif /* __BSP_TCP_H */


/*------------------------------- END OF FILE -------------------------------*/


