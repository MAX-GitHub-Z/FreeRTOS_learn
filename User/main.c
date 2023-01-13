/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS v9.0.0 + STM32 工程模版
  *********************************************************************
  * @attention
  *
  * 实验平台:野火 STM32全系列开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/ 
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"


/* 开发板硬件bsp头文件 */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"


/*常用C语言头文件*/
#include <stdio.h>
#include <string.h>
/**************************** 任务句柄 ********************************/
 /*
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为 NULL。
 */
/*创建任务句柄*/
static TaskHandle_t AppTaskCreate_Handle=NULL;
/*LED1任务句柄*/
static TaskHandle_t LED1_Task_Handle=NULL;
/*LED1任务句柄*/
static TaskHandle_t LED2_Task_Handle=NULL;
/*按键任务句柄*/
static TaskHandle_t KEY_Task_Handle=NULL;

/*********************************
 * 全局变量
 * 
 */

/*
***************************************************************
 * 硬件初始化
 * BSP_init
 * 板子上的所有硬件初始化应该放到这个函数里面
 * 无返回值
 * *************************************************************
 * */
void BSP_init()
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//STM32中断优先级分组为4
 LED_GPIO_Config(); //LED端口初始化
 Key_GPIO_Config(); //按键端口初始化
 USART_Config(); //USART串口初始化
}
/*
***********************************************************
 * 定义一个LED闪烁的任务
 * LED1_TASK
 * LED每500ms翻转一次状态
 * 无返回值
 * ********************************************************
 * */

static void LED1_Task(void* parameter)
{
  while(1)
  {
   // printf("LED1_ON!\r\n");
    LED1_ON;
    vTaskDelay(500);/*延时500个tick*/
    //printf("LED1_OFF!\r\n");
    LED1_OFF;
    vTaskDelay(500);
  }
}
/*
***********************************************************
 * 定义一个LED闪烁的任务
 * LED2_TASK
 * LED每1s翻转一次状态
 * 无返回值
 * ********************************************************
 * */

static void LED2_Task(void* parameter)
{
  while(1)
  {
    //printf("LED2_ON!\r\n");
    LED2_ON;
    vTaskDelay(1000);/*延时500个tick*/
    //printf("LED2_OFF!\r\n");
    LED2_OFF;
    vTaskDelay(1000);
  }
}
/********************************
 * 定义一个按键控制任务挂起就绪的任务
 * 按下按键K1，任务LED1挂起LED2恢复
 * 按下按键K2，任务LED2挂起LED1恢复
 * 
 ********************************
 * */
static void KEY_Task(void* parameter)
{
  while (1)
  {
    if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) ==KEY_ON)
    {
      /*K1按键被按下*/
      vTaskSuspend(LED1_Task_Handle);
      vTaskResume(LED2_Task_Handle);
      printf("挂起LED1任务!\r\n恢复LED2任务!\r\n");
    }
    if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN)==KEY_ON)
    {
      /*K2按键被按下*/
      vTaskSuspend(LED2_Task_Handle);
      vTaskResume(LED1_Task_Handle);
      printf("挂起LED2任务!\r\n恢复LED1任务!\r\n");
    }
    vTaskDelay(20);//延时20个tick
  }
  

}


/**
 * @brief 为了方便管理，一般所有任务创建函数都放在这里
 * 
 */
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;//定义一个创建信息返回值，
  taskENTER_CRITICAL();//进入临界区
  /*创建LED1_Task任务*/
  xReturn=xTaskCreate((TaskFunction_t )LED1_Task,//任务函数
                      (const char*)"LED1_Task",//
                      (uint32_t   )512,//
                      (void*      )NULL,//传递给任务的参数
                      (UBaseType_t)2,//任务优先级
                      (TaskHandle_t*)&LED1_Task_Handle);//任务控制块
  if(pdPASS==xReturn)
    printf("LED1_Task任务创建成功\r\n");
  else
    printf("LED1_Task任务创建失败\r\n");
    /*创建LED2_Task任务*/
  xReturn=xTaskCreate((TaskFunction_t )LED2_Task,//任务函数
                      (const char*)"LED2_Task",//
                      (uint32_t   )512,//
                      (void*      )NULL,//传递给任务的参数
                      (UBaseType_t)3,//任务优先级
                      (TaskHandle_t*)&LED2_Task_Handle);//任务控制块
  if(pdPASS==xReturn)
    printf("LED2_Task任务创建成功\r\n");
  else
    printf("LED2_Task任务创建失败\r\n");

    /*创建KEY_Task任务*/
  xReturn=xTaskCreate((TaskFunction_t )KEY_Task,//任务函数
                      (const char*)"KEY_Task",//
                      (uint32_t   )512,//
                      (void*      )NULL,//传递给任务的参数
                      (UBaseType_t)1,//任务优先级
                      (TaskHandle_t*)&KEY_Task_Handle);//任务控制块
  if(pdPASS==xReturn)
    printf("KEY_Task任务创建成功\r\n");
  else
    printf("KEY_Task任务创建失败\r\n");




  vTaskDelete(AppTaskCreate_Handle);//删除AppTaskCreate_Handle任务
  taskEXIT_CRITICAL();//退出临界区

}
int main(void)
{	
  BaseType_t xReturn =pdPASS; //定义一个创建信息的返回值，默认为pdPASS
  BSP_init();/*开发板硬件初始化*/
  printf("这是一个基于STM32F103的FreeRTOS系统的动态多任务创建的测试!\r\n");
  /*创建AppTaskCreate任务*/
 xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate,
                                      (const char*   )"AppTaskCreate",//任务名称
                                      (uint32_t      )512,//任务堆栈大小
                                      (void*         )NULL,//传递给任务函数的参数
                                      (UBaseType_t   )1,//任务优先级
                                      (TaskHandle_t*  )&AppTaskCreate_Handle);  
if(NULL!=xReturn)/*创建成功*/
    vTaskStartScheduler();/*启动任务，开启调度*/
else
   return 1;


while(1);
}

/********************************END OF FILE****************************/
