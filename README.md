# ESP8266-RTOS-SDK-demo
esp8266 rtos sdk v1.5 源码分析

	基于机智云开发板开发学习



### 程序简介

+ bsp文件为自开发代码

+ 编写了按键、LED pwm控制、tcp/udp、串口处理框架

+ 已为常用的函数添加了相关的注释

+ 关于 Make文件学习也添加了相应的注释和说明

+ 学习专栏：https://blog.csdn.net/qq_42992084/category_9369657.html



### 项目构建

+ 开发环境搭建：https://docs.ai-thinker.com/ai_ide_use

+ 项目属性的一些重要配置：

  ![part1](..\ESP8266_RTOS_SDK-1.5.0\png\part1.png)

  ![part2](..\ESP8266_RTOS_SDK-1.5.0\png\part2.png)

  编译规则默认填写：（请根据个人情况进行选择）

  ```
  COMPILE=gcc BOOT=new APP=1 SPI_SPEED=40 SPI_MODE=QIO SPI_SIZE_MAP=4
  ```

+ 首次 Build Project （编译项目），请先 Clean Project（清理项目）



### 微信 airkiss局域网发现

​		参见项目案例里的 smart_config ，需要说明的是必须配置正确的 设备类型（微信id） 和 设备id 才可以被微信内网发现到，可以申请个微信测试号（https://mp.weixin.qq.com/debug/cgi-bin/sandbox?t=sandbox/login）进行测试，相关开发文档 http://iot.weixin.qq.com/wiki/new/index.html?page=4-1-2



### 下载配置

![download_32Mbit](..\ESP8266_RTOS_SDK-1.5.0\download_32Mbit.png)

所用 bin相关文件在主目录 bin文件夹下，编译后的 bin文件在当前目录的 upgrade文件夹下

