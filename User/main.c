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


/**************************** 任务句柄 ********************************/
 /*
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为 NULL。
 */
/*创建任务句柄*/
static TaskHandle_t AppTaskCreate_Handle;
/*LED任务句柄*/
static TaskHandle_t LED_Task_Handle;
/********************************全局变量声明********************************/

/*AppTaskCreate任务堆栈*/
static StackType_t AppTaskCreate_Stack[128];
/*LED任务堆栈*/
static StackType_t LED_Task_Stack[128];

/*AppTaskCreate任务控制块*/
static StaticTask_t AppTaskCreate_TCB;
/*LED任务控制块*/
static StaticTask_t LED_Task_TCB;

/*空闲任务堆栈*/
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
/*定时器任务堆栈*/
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];
/*空闲任务控制块*/
static StaticTask_t Idle_Task_TCB;
/*定时器任务控制块*/
static StaticTask_t Timer_Task_TCB;



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
 USART_Config(); //USART串口初始化
}
/*
***********************************************************
 * 定义一个LED闪烁的任务
 * LED_TASK
 * LED每500ms翻转一次状态
 * 无返回值
 * ********************************************************
 * */

static void LED_Task(void* parameter)
{
  while(1)
  {
    printf("LED_ON!\r\n");
    LED1_ON;
    vTaskDelay(500);/*延时500个tick*/
    printf("LED_OFF!\r\n");
    LED1_OFF;
    vTaskDelay(500);
  }
}


/**
 *************************************************************
 * 获取空闲任务的任务堆栈和任务控制块内存
 * ppxIdleTaskTCBBuffer     :任务块内存
 * ppxIDleTaskStackBuffer   ：任务堆栈内存
 * pulIdleTaskStackSize     ：任务堆栈大小
 * @return viod 
 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIDleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
  *ppxIdleTaskTCBBuffer=&Idle_Task_TCB;/*任务控制块内存*/
  *ppxIDleTaskStackBuffer=Idle_Task_Stack;/*任务堆栈内存*/
  *pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;/*任务堆栈大小*/
}
/**
 * @brief 获取定时器任务的任务堆栈和任务控制块内存
 * ppxTimerTaskTCBBuffer    ：任务控制块内存
 * ppxTimerTaskStackBuffer  ：任务堆栈内存
 * pulTimerTaskStackSize    ：任务堆栈大小
 * @return void 
 */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
  *ppxTimerTaskTCBBuffer=&Timer_Task_TCB;/* 任务控制块内存 */
  *ppxTimerTaskStackBuffer=Timer_Task_Stack;/* 任务堆栈内存 */
  *pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;/* 任务堆栈大小 */
}

/**
 * @brief 为了方便管理，一般所有任务创建函数都放在这里
 * 
 */
static void AppTaskCreate(void)
{
  taskENTER_CRITICAL();//进入临界区
  LED_Task_Handle=xTaskCreateStatic((TaskFunction_t )LED_Task,//任务函数
                                    (const char*)"LED_Task",//
                                    (uint32_t   )128,//
                                    (void*      )NULL,//传递给任务的参数
                                    (UBaseType_t)4,//任务优先级
                                    (StackType_t*)LED_Task_Stack,//任务堆栈
                                    (StaticTask_t*)&LED_Task_TCB);//任务控制块
  if(NULL!=LED_Task_Handle)
    printf("LED_Task_Handle任务创建成功\r\n");
  else
    printf("LED_Task_Handle任务创建失败\r\n");
  
  vTaskDelete(AppTaskCreate_Handle);//删除AppTaskCreate_Handle任务
  taskEXIT_CRITICAL();//退出临界区

}
int main(void)
{	
  BSP_init();/*开发板硬件初始化*/
  printf("这是一个基于STM32F103的FreeRTOS系统的静态创建任务的测试!\r\n");
  /*创建AppTaskCreate任务*/
  AppTaskCreate_Handle=xTaskCreateStatic((TaskFunction_t)AppTaskCreate,
                                      (const char*   )"AppTaskCreate",//任务名称
                                      (uint32_t      )128,//任务堆栈大小
                                      (void*         )NULL,//传递给任务函数的参数
                                      (UBaseType_t   )3,//任务优先级
                                      (StackType_t*  )AppTaskCreate_Stack,
                                      (StaticTask_t*)&AppTaskCreate_TCB);  
if(NULL!=AppTaskCreate_Handle)/*创建成功*/
    vTaskStartScheduler();/*启动任务，开启调度*/



while(1);
}

/********************************END OF FILE****************************/
