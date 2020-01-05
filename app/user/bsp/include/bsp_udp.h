/*
 * bsp_udp.h
 *
 *  Created on: 2019年9月5日
 *      Author: liziyuan
 */

#ifndef __BSP_UDP_H
#define __BSP_UDP_H


#include "../../../include/udp.h"

#define USER_UDP_PORT			8080

extern struct espconn user_udp_conn;
extern esp_udp user_udp;

void ICACHE_FLASH_ATTR user_UDP_Send(void *arg);
void ICACHE_FLASH_ATTR user_UDP_Reveive(void *arg, char *pusrdata, unsigned short length);
void ICACHE_FLASH_ATTR udp_communication_task(void *pvParameters);


#endif /* __BSP_UDP_H */


/*------------------------------- END OF FILE -------------------------------*/


