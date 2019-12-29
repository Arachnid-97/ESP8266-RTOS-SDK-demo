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

#ifndef __KEY_H_
#define __KEY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"


typedef void (* key_function)(void);

struct single_key_param {
    uint8 key_level;			/*按键处理流程标号*/
    uint8 gpio_id;				/*GPIO引脚号（0-15*/
    uint8 gpio_func;			/*GPIO 功能(详见ESP8266 管脚清单.xlsx)*/
    uint32 gpio_name;			/*GPIO MUX寄存器（参见gpio.h）*/
    os_timer_t key_5s;			/*定时器相关数据结构(详见SDK开发文档)*/
    os_timer_t key_50ms;		/*定时器相关数据结构(详见SDK开发文档)*/
    key_function short_press;	/*按键短按处理程序*/
    key_function long_press;	/*按键长按处理程序*/
};

struct keys_param {
    uint8 key_num;				/*按键的数量*/
    struct single_key_param **single_key;	/*按键的相关信息*/
};

struct single_key_param *key_init_single(uint8 gpio_id, uint32 gpio_name, uint8 gpio_func, key_function long_press, key_function short_press);
void key_init(struct keys_param *key);

#ifdef __cplusplus
}
#endif

#endif /* __KEY_H_ */
