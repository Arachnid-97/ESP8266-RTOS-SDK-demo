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
#define USER_TCP_CLIENT_PORT		8080

void ICACHE_FLASH_ATTR user_TCP_Reveive(void);
void ICACHE_FLASH_ATTR user_TCP_Send(void);
void ICACHE_FLASH_ATTR user_TCP_Abnormal(void);
void ICACHE_FLASH_ATTR tcp_communication_task(void *pvParameters);


#endif /* __BSP_TCP_H */


/*------------------------------- END OF FILE -------------------------------*/


