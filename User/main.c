/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS v9.0.0 + STM32 ����ģ��
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32ȫϵ�п����� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"


/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_usart.h"


/**************************** ������ ********************************/
 /*
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������Ϊ NULL��
 */
/*����������*/
static TaskHandle_t AppTaskCreate_Handle;
/*LED������*/
static TaskHandle_t LED_Task_Handle;
/********************************ȫ�ֱ�������********************************/

/*AppTaskCreate�����ջ*/
static StackType_t AppTaskCreate_Stack[128];
/*LED�����ջ*/
static StackType_t LED_Task_Stack[128];

/*AppTaskCreate������ƿ�*/
static StaticTask_t AppTaskCreate_TCB;
/*LED������ƿ�*/
static StaticTask_t LED_Task_TCB;

/*���������ջ*/
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
/*��ʱ�������ջ*/
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];
/*����������ƿ�*/
static StaticTask_t Idle_Task_TCB;
/*��ʱ��������ƿ�*/
static StaticTask_t Timer_Task_TCB;



/*
***************************************************************
 * Ӳ����ʼ��
 * BSP_init
 * �����ϵ�����Ӳ����ʼ��Ӧ�÷ŵ������������
 * �޷���ֵ
 * *************************************************************
 * */
void BSP_init()
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//STM32�ж����ȼ�����Ϊ4
 LED_GPIO_Config(); //LED�˿ڳ�ʼ��
 USART_Config(); //USART���ڳ�ʼ��
}
/*
***********************************************************
 * ����һ��LED��˸������
 * LED_TASK
 * LEDÿ500ms��תһ��״̬
 * �޷���ֵ
 * ********************************************************
 * */

static void LED_Task(void* parameter)
{
  while(1)
  {
    printf("LED_ON!\r\n");
    LED1_ON;
    vTaskDelay(500);/*��ʱ500��tick*/
    printf("LED_OFF!\r\n");
    LED1_OFF;
    vTaskDelay(500);
  }
}


/**
 *************************************************************
 * ��ȡ��������������ջ��������ƿ��ڴ�
 * ppxIdleTaskTCBBuffer     :������ڴ�
 * ppxIDleTaskStackBuffer   �������ջ�ڴ�
 * pulIdleTaskStackSize     �������ջ��С
 * @return viod 
 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIDleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
  *ppxIdleTaskTCBBuffer=&Idle_Task_TCB;/*������ƿ��ڴ�*/
  *ppxIDleTaskStackBuffer=Idle_Task_Stack;/*�����ջ�ڴ�*/
  *pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;/*�����ջ��С*/
}
/**
 * @brief ��ȡ��ʱ������������ջ��������ƿ��ڴ�
 * ppxTimerTaskTCBBuffer    ��������ƿ��ڴ�
 * ppxTimerTaskStackBuffer  �������ջ�ڴ�
 * pulTimerTaskStackSize    �������ջ��С
 * @return void 
 */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
  *ppxTimerTaskTCBBuffer=&Timer_Task_TCB;/* ������ƿ��ڴ� */
  *ppxTimerTaskStackBuffer=Timer_Task_Stack;/* �����ջ�ڴ� */
  *pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;/* �����ջ��С */
}

/**
 * @brief Ϊ�˷������һ���������񴴽���������������
 * 
 */
static void AppTaskCreate(void)
{
  taskENTER_CRITICAL();//�����ٽ���
  LED_Task_Handle=xTaskCreateStatic((TaskFunction_t )LED_Task,//������
                                    (const char*)"LED_Task",//
                                    (uint32_t   )128,//
                                    (void*      )NULL,//���ݸ�����Ĳ���
                                    (UBaseType_t)4,//�������ȼ�
                                    (StackType_t*)LED_Task_Stack,//�����ջ
                                    (StaticTask_t*)&LED_Task_TCB);//������ƿ�
  if(NULL!=LED_Task_Handle)
    printf("LED_Task_Handle���񴴽��ɹ�\r\n");
  else
    printf("LED_Task_Handle���񴴽�ʧ��\r\n");
  
  vTaskDelete(AppTaskCreate_Handle);//ɾ��AppTaskCreate_Handle����
  taskEXIT_CRITICAL();//�˳��ٽ���

}
int main(void)
{	
  BSP_init();/*������Ӳ����ʼ��*/
  printf("����һ������STM32F103��FreeRTOSϵͳ�ľ�̬��������Ĳ���!\r\n");
  /*����AppTaskCreate����*/
  AppTaskCreate_Handle=xTaskCreateStatic((TaskFunction_t)AppTaskCreate,
                                      (const char*   )"AppTaskCreate",//��������
                                      (uint32_t      )128,//�����ջ��С
                                      (void*         )NULL,//���ݸ��������Ĳ���
                                      (UBaseType_t   )3,//�������ȼ�
                                      (StackType_t*  )AppTaskCreate_Stack,
                                      (StaticTask_t*)&AppTaskCreate_TCB);  
if(NULL!=AppTaskCreate_Handle)/*�����ɹ�*/
    vTaskStartScheduler();/*�������񣬿�������*/



while(1);
}

/********************************END OF FILE****************************/
