# FreeRTOS_learn
这个项目是基于野火的STM32F103ZET6霸道开发板实现的，主要目的是学习FreeRTOS操作系统和熟悉项目上传GitHub的过程。
我会在学习的过程中完善这个项目，即在STM32F103中运行FreeRTOS并尽可能的将开发板上的内容可以根据目录的选择来开启对应的模块
注意：这个是基于STM32F103的固件库开发的
# 2023年1月9日
 根据野火相关教程，完成了动态多任务的创建即LED1每500ms翻转一次 LED2每1S翻转一次（第一个版本的提交）
注意：在这个例程中LED1使用的引脚为PB5，LED2使用的引脚为PB0，如果需要修改，请在“bsp_led.h”这个文件中修改相关引脚与时钟
# 2023年1月13日
  根据野火相关教程完成了任务管理，即任务的挂起与恢复，实现了按下KEY1按键时，挂起LED1任务，恢复LED2任务，按下KEY2按键时，挂起LED2任务，恢复LED1任务.、
注意：本例程使用的LED引脚与上一个相同，如果需要修改，请在“bsp_led.h”这个文件中修改相关引脚与时钟
注意：在这个例程中KEY1使用的引脚为PA0，KEY2使用PC13引脚，如果需要修改，请在“bsp_key.h”这个文件中修改相关引脚