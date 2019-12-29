/*
 * bsp_led.c
 *
 *  Created on: 2019年8月30日
 *      Author: liziyuan
 */

#include "esp_common.h"

#include "bsp_led.h"


os_timer_t WifiLED_250ms;

/******************************************************************************
 * FunctionName : wifiled_state_cb
 * Description  : led状态定时回调
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR wifiled_state_cb(void)
{
	LOCAL uint8 wifi_status = 1;

	if (wifi_station_get_connect_status() == STATION_GOT_IP)	// 网络正常则绿灯常亮
	{
//		os_timer_disarm(&WifiLED_250ms);
		RLED_OFF();
		GLED_ON();
	}
	else			// 断网闪烁
	{
		if(wifi_status)
		{
			wifi_status = 0;
			RLED_ON();
		}
		else
		{
			wifi_status = 1;
			RLED_OFF();
		}
	}
}


void ICACHE_FLASH_ATTR RLED_ON(void)
{
	GPIO_OUTPUT_SET(15, 1);		//GPIO15输出高电平
}

void ICACHE_FLASH_ATTR GLED_ON(void)
{
//	GPIO_OUTPUT_SET(12, 1);		//GPIO12输出高电平

	/* PWM输出 */
	pwm_set_duty(100, 0);			// 重配置 LED PWM占空比、占用通道0
//	pwm_set_period(450);			// 重设置周期
	pwm_start();					// LED闪烁启动
}

void ICACHE_FLASH_ATTR BLED_ON(void)
{
	GPIO_OUTPUT_SET(13, 1);		//GPIO13输出高电平
}

void ICACHE_FLASH_ATTR RLED_OFF(void)
{
	GPIO_OUTPUT_SET(15, 0);		//GPIO15输出低电平
}

void ICACHE_FLASH_ATTR GLED_OFF(void)
{
//	GPIO_OUTPUT_SET(12, 0);		//GPIO12输出低电平

	pwm_set_duty(0, 0);
	pwm_start();
}

void ICACHE_FLASH_ATTR BLED_OFF(void)
{
	GPIO_OUTPUT_SET(13, 0);		//GPIO13输出低电平
}

/******************************************************************************
 * FunctionName : rled_gpio_config
 * Description  : red(GPIO15)配置
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR rled_gpio_config(void)
{
	GPIO_ConfigTypeDef pGPIOConfig;

	pGPIOConfig.GPIO_IntrType = GPIO_PIN_INTR_DISABLE;
	pGPIOConfig.GPIO_Pullup = GPIO_PullUp_EN;
	pGPIOConfig.GPIO_Mode = GPIO_Mode_Output;
	pGPIOConfig.GPIO_Pin = USER_RLED_GPIO_PIN;			// rled
	gpio_config(&pGPIOConfig);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(USER_RLED_GPIO), 0);	// close rled
}

/******************************************************************************
 * FunctionName : gled_gpio_config
 * Description  : green(GPIO12)配置
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR gled_gpio_config(void)
{
	GPIO_ConfigTypeDef pGPIOConfig;

	pGPIOConfig.GPIO_IntrType = GPIO_PIN_INTR_DISABLE;
	pGPIOConfig.GPIO_Pullup = GPIO_PullUp_EN;
	pGPIOConfig.GPIO_Mode = GPIO_Mode_Output;
	pGPIOConfig.GPIO_Pin = USER_GLED_GPIO_PIN;			// gled
	gpio_config(&pGPIOConfig);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(USER_GLED_GPIO), 0);	// close gled
}

/******************************************************************************
 * FunctionName : bled_gpio_config
 * Description  : blue(GPIO13)配置
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR bled_gpio_config(void)
{
	GPIO_ConfigTypeDef pGPIOConfig;

	pGPIOConfig.GPIO_IntrType = GPIO_PIN_INTR_DISABLE;
	pGPIOConfig.GPIO_Pullup = GPIO_PullUp_EN;
	pGPIOConfig.GPIO_Mode = GPIO_Mode_Output;
	pGPIOConfig.GPIO_Pin = USER_BLED_GPIO_PIN;			// bled
	gpio_config(&pGPIOConfig);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(USER_BLED_GPIO), 0);	// close bled
}

/******************************************************************************
 * FunctionName : led_pwm_init
 * Description  : led PWM初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR led_pwm_init(void)
{
	/* 初始化 PWM，周期, pwm_duty_init占空比, 3通道数, 通道 IO */
	uint32 io_info[][3] ={	{PWM_0_OUT_IO_MUX,PWM_0_OUT_IO_FUNC,PWM_0_OUT_IO_NUM},
							{PWM_1_OUT_IO_MUX,PWM_1_OUT_IO_FUNC,PWM_1_OUT_IO_NUM},
							{PWM_2_OUT_IO_MUX,PWM_2_OUT_IO_FUNC,PWM_2_OUT_IO_NUM}	};
	uint32 pwm_duty_init[3] = {0, 0, 0};

//	pwm_init(1000, pwm_duty_init,3,io_info);	// 周期1000us，也就是1khz的频率，各通道初始占空比0

	pwm_init(450, &pwm_duty_init[1], 1, &io_info[1]);	// 只占用 1个通道、设定IO 12绿灯

	/* 重设占空比。最大占空比在 pwm.h 里设置 8191
	pwm_set_duty(duty, channel);   	// 0 ~ 2(三通道)
	pwm_set_period(uint32 period);  // set period
	pwm_start();
	*/
}

/******************************************************************************
 * FunctionName : uesr_LED_Init
 * Description  : LED初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR uesr_LED_Init(void)
{
	rled_gpio_config();
	gled_gpio_config();
	bled_gpio_config();

	led_pwm_init();
}


/*------------------------------- END OF FILE -------------------------------*/


