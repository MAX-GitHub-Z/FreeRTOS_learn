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
#include "semphr.h"//二值信号量头文件

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
/*串口数据接收处理句柄*/
static TaskHandle_t USART_Task_Handle=NULL;
/*接收消息句柄*/
static TaskHandle_t KEY2_Task_Handle = NULL;
/*发送消息句柄*/
static TaskHandle_t KEY1_Task_Handle = NULL;


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


//QueueHandle_t Test_Queue =NULL;
SemaphoreHandle_t BinarySem_Handle =NULL;//二值信号量句柄


 /*************************** 宏定义 ************************************/
 /*
 * 当我们在写应用程序的时候，可能需要用到一些宏定义。
 */
 #define QUEUE_LEN 1 /* 队列的长度，最大可包含多少个消息 */
 #define QUEUE_SIZE 4 /* 队列中每个消息大小（字节） */




char Usart_Rx_Buf[USART_RBUFF_SIZE];//串口接收数据数组


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
/**
 * 串口处理程序
 * 
 */
static void USART_Task(void* parameter)
{
  char* data=Usart_Rx_Buf;
  while (1)
  {
    //printf("数据%s:",data);
    /* code */
  }
  

}



/*
***********************************************************
 * 按下KEY1获取二值信号量
 * ********************************************************
 * */

static void KEY1_Task(void* parameter)
{
  BaseType_t xSemaphore=NULL;
  while(1)
  {
    if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )
    {
      xSemaphore=xSemaphoreTake(BinarySem_Handle,0);//获取二值信号量
      if(xSemaphore==pdPASS)
        printf("获取二值信号量成功\r\n");
      else
        printf("获取二值信号量失败\r\n");
    }
    vTaskDelay(10);
  }
}

/********************************
 * 按下KEY2释放二值信号量
 * 
 ********************************
 * */
static void KEY2_Task(void* parameter)
{
  BaseType_t xSemaphore=NULL;
  while(1)
  {
    if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) ==KEY_ON)
    {
      xSemaphore=xSemaphoreGive(BinarySem_Handle);//释放二值信号量
      if(xSemaphore==pdTRUE)
        printf("释放二值信号量成功\r\n");
      else
        printf("释放二值信号量失败\r\n");
    }
    vTaskDelay(100);
  }
}

/**
 * @brief 为了方便管理，一般所有任务创建函数都放在这里
 * 
 */
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;//定义一个创建信息返回值
  
  taskENTER_CRITICAL();//进入临界区
  BinarySem_Handle = xSemaphoreCreateBinary();
  if(BinarySem_Handle == NULL)
      printf("二值信号量创建失败\r\n");
  else
      printf("二值信号量创建成功\r\n");

  /*创建KEY1_Task任务*/
  xReturn=xTaskCreate((TaskFunction_t )KEY1_Task,//任务函数
                      (const char*)"KEY1_Task",//
                      (uint32_t   )512,//
                      (void*      )NULL,//传递给任务的参数
                      (UBaseType_t)2,//任务优先级
                      (TaskHandle_t*)&KEY1_Task_Handle);//任务控制块
  if(pdPASS==xReturn)
    printf("KEY1_Task任务创建成功\r\n");
  else
    printf("KEY1_Task任务创建失败\r\n");
    /*创建KEY2_Task任务*/
  xReturn=xTaskCreate((TaskFunction_t )KEY2_Task,//任务函数
                      (const char*)"KEY2_Task",//
                      (uint32_t   )512,//
                      (void*      )NULL,//传递给任务的参数
                      (UBaseType_t)3,//任务优先级
                      (TaskHandle_t*)&KEY2_Task_Handle);//任务控制块
  if(pdPASS==xReturn)
    printf("KEY2_Task任务创建成功\r\n");
  else
    printf("KEY2_Task任务创建失败\r\n");

 /*创建USART_Task任务*/
  xReturn=xTaskCreate((TaskFunction_t )USART_Task,//任务函数
                      (const char*)"USART_Task",//
                      (uint32_t   )512,//
                      (void*      )NULL,//传递给任务的参数
                      (UBaseType_t)1,//任务优先级
                      (TaskHandle_t*)&USART_Task_Handle);//任务控制块
  if(pdPASS==xReturn)
    printf("USART_Task任务创建成功\r\n");
  else
    printf("USART_Task任务创建失败\r\n");

  vTaskDelete(AppTaskCreate_Handle);//删除AppTaskCreate_Handle任务
  taskEXIT_CRITICAL();//退出临界区

}
int main(void)
{	
  BaseType_t xReturn =pdPASS; //定义一个创建信息的返回值，默认为pdPASS
  BSP_init();/*开发板硬件初始化*/
  printf("这是一个基于STM32F103的FreeRTOS系统的二值信号量的测试!\r\n");
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
