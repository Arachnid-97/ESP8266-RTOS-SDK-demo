/*
 * bsp_udp.h
 *
 *  Created on: 2019年9月5日
 *      Author: liziyuan
 */

#ifndef __BSP_UDP_H
#define __BSP_UDP_H


#include "../../../include/udp.h"

#define USER_UDP_PORT			8000

void ICACHE_FLASH_ATTR udp_communication_task(void *pvParameters);


#endif /* __BSP_UDP_H */


/*------------------------------- END OF FILE -------------------------------*/


