/*
 * bsp_uart.c
 *
 *  Created on: 2019年9月7日
 *      Author: liziyuan
 */

#include "esp_common.h"

#include "bsp_uart.h"


xQueueHandle xQueueCusUart;
xQueueHandle xQueueXmodem;

/******************************************************************************
 * FunctionName : uart0_send_data
 * Description  : uart 0数据发送
 * Parameters   : Buf ---- 数据
 * 				  Len ---- 长度
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR uart0_send_data(uint8 *Buf, uint32 Len)
{
	uint32 i;

	for(i = 0;i < Len;i++)
	{
		WRITE_PERI_REG(UART_FIFO(UART0) , *(Buf + i));		// 写一个字节到串口 0的FIFO
	}
}

/******************************************************************************
 * FunctionName : uart_receive
 * Description  : uart数据接收处理
 * Parameters   : Buf ---- 数据
 * 				  Len ---- 长度
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR uart_receive(uint8 *Buf, uint32 Len)
{
	uart0_send_data(Buf, Len);
}

/******************************************************************************
 * FunctionName : uart_task
 * Description  : uart处理任务
 * Parameters   : pvParameters
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR uart_task(void *pvParameters)
{
	UART_Buf_Type Uart0_data;

	while(1)
	{
		if(xQueueReceive(xQueueCusUart, (void *)&Uart0_data, (portTickType)500/*portMAX_DELAY*/))	// wait about 5sec
		{
			if(Uart0_data.len > 0)
			{
				uart_receive(Uart0_data.buf, Uart0_data.len);
			}
		}
//		vTaskDelay(50 / portTICK_RATE_MS);	 // 50 ms
	}

	vTaskDelete(NULL);
}

/******************************************************************************
 * FunctionName : user_Uart_Init
 * Description  : 串口初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_Uart_Init(void)
{
	xQueueCusUart = xQueueCreate((unsigned portBASE_TYPE)UART0_QUEUE_LENGTH, sizeof(UART_Buf_Type));	// 目标队列的句柄
//	xQueueXmodem = xQueueCreate((unsigned portBASE_TYPE)(1), sizeof(UART_Buf_Type));

	uart_init_new();	// config uart0 connection device MCU, config uart1 TX debug output

	xTaskCreate(uart_task, "uart_task", 256, NULL, tskIDLE_PRIORITY + 4, NULL);	// 串口处理任务
}


/*------------------------------- END OF FILE -------------------------------*/


