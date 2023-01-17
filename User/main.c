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
#include "queue.h"

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
/*接收消息句柄*/
static TaskHandle_t Receive_Task_Handle = NULL;
/*发送消息句柄*/
static TaskHandle_t Send_Task_Handle = NULL;


/***************************** 内核对象句柄 *****************************/
 /*
 * 信号量，消息队列，事件标志组，软件定时器这些都属于内核的对象，要想使用这些内核
 * 对象，必须先创建，创建成功之后会返回一个相应的句柄。实际上就是一个指针，后续我
 * 们就可以通过这个句柄操作这些内核对象。
 *
 *
 内核对象说白了就是一种全局的数据结构，通过这些数据结构我们可以实现任务间的通信，
 * 任务间的事件同步等各种功能。至于这些功能的实现我们是通过调用这些内核对象的函数
 * 来完成的
 *
 */


QueueHandle_t Test_Queue =NULL;



 /*************************** 宏定义 ************************************/
 /*
 * 当我们在写应用程序的时候，可能需要用到一些宏定义。
 */
 #define QUEUE_LEN 4 /* 队列的长度，最大可包含多少个消息 */
 #define QUEUE_SIZE 4 /* 队列中每个消息大小（字节） */
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
 * 接收消息并打印
 * ********************************************************
 * */

static void Receive_Task(void* parameter)
{
  BaseType_t xReturn = pdTRUE; //定义一个创建信息返回值，默认为pdTRUE
  uint32_t r_queue;//定义一个接收消息的变量
  while(1)
  {
    xReturn =xQueueReceive(Test_Queue,//消息队列句柄
                          &r_queue,//发送消息内容
                          portMAX_DELAY);//等待时间，一直等
    if(pdTRUE==xReturn)
      printf("本次接收到的数据是%d\n",r_queue);
    else
      printf("数据接收出错,错误代码: 0x%lx\n",xReturn);
  }
}

/********************************
 * 定义一个按键控制任务挂起就绪的任务
 * 按下按键K1，任务LED1挂起LED2恢复
 * 按下按键K2，任务LED2挂起LED1恢复
 * 
 ********************************
 * */
static void Send_Task(void* parameter)
{
  BaseType_t xReturn=pdPASS;//定义一个创建信息返回值，默认为paASS
  uint32_t send_data1=1;
  uint32_t send_data2=2;
  while (1)
  {
    if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) ==KEY_ON)
    {
      /*K1按键被按下*/
      printf("发送消息send_data1\r\n");
      xReturn=xQueueSend(Test_Queue,//消息队列句柄 
                       &send_data1,//发送消息内容
                        0 );//等待时间0
			if(pdPASS == xReturn)
        printf("消息send_data1发送成功!\n\n");
    }
    if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN)==KEY_ON)
    {
      /*K2按键被按下*/
      
     printf("发送消息send_data2\r\n");
      xReturn=xQueueSend(Test_Queue,//消息队列句柄 
                       &send_data2,//发送消息内容
                        0 );//等待时间0
			if(pdPASS == xReturn)
        printf("消息send_data2发送成功!\n\n");
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
  /*创建消息队列Test_Queue*/
  Test_Queue=xQueueCreate((UBaseType_t)QUEUE_LEN,//消息队列的长度
                          (UBaseType_t)QUEUE_SIZE);//消息的大小
  /*创建Receive_Task任务*/
  xReturn=xTaskCreate((TaskFunction_t )Receive_Task,//任务函数
                      (const char*)"Receive_Task",//
                      (uint32_t   )512,//
                      (void*      )NULL,//传递给任务的参数
                      (UBaseType_t)2,//任务优先级
                      (TaskHandle_t*)&Receive_Task_Handle);//任务控制块
  if(pdPASS==xReturn)
    printf("Receive_Task任务创建成功\r\n");
  else
    printf("Receive_Task任务创建失败\r\n");
    /*创建Send_Task任务*/
  xReturn=xTaskCreate((TaskFunction_t )Send_Task,//任务函数
                      (const char*)"Send_Task",//
                      (uint32_t   )512,//
                      (void*      )NULL,//传递给任务的参数
                      (UBaseType_t)3,//任务优先级
                      (TaskHandle_t*)&Send_Task_Handle);//任务控制块
  if(pdPASS==xReturn)
    printf("Send_Task任务创建成功\r\n");
  else
    printf("Send_Task任务创建失败\r\n");


  vTaskDelete(AppTaskCreate_Handle);//删除AppTaskCreate_Handle任务
  taskEXIT_CRITICAL();//退出临界区

}
int main(void)
{	
  BaseType_t xReturn =pdPASS; //定义一个创建信息的返回值，默认为pdPASS
  BSP_init();/*开发板硬件初始化*/
  printf("这是一个基于STM32F103的FreeRTOS系统的消息队列的测试!\r\n");
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
