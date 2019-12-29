/*
 * bsp_uart.h
 *
 *  Created on: 2019年9月7日
 *      Author: liziyuan
 */

#ifndef __BSP_UART_H
#define __BSP_UART_H


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "../../../include/uart.h"

#define UART0_QUEUE_LENGTH		3
#define USER_UART_MAX			50

typedef struct
{
	uint16 len;
	uint8 buf[USER_UART_MAX];
}UART_Buf_Type;

extern xQueueHandle xQueueCusUart;
extern xQueueHandle xQueueXmodem;

void ICACHE_FLASH_ATTR user_Uart_Init(void);


#endif /* __BSP_UART_H */


/*------------------------------- END OF FILE -------------------------------*/


