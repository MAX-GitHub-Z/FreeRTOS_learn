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
static TaskHandle_t AppTaskCreate_Handle=NULL;
/*LED1������*/
static TaskHandle_t LED1_Task_Handle=NULL;
/*LED1������*/
static TaskHandle_t LED2_Task_Handle=NULL;



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
 * LED1_TASK
 * LEDÿ500ms��תһ��״̬
 * �޷���ֵ
 * ********************************************************
 * */

static void LED1_Task(void* parameter)
{
  while(1)
  {
    printf("LED1_ON!\r\n");
    LED1_ON;
    vTaskDelay(500);/*��ʱ500��tick*/
    printf("LED1_OFF!\r\n");
    LED1_OFF;
    vTaskDelay(500);
  }
}
/*
***********************************************************
 * ����һ��LED��˸������
 * LED2_TASK
 * LEDÿ1s��תһ��״̬
 * �޷���ֵ
 * ********************************************************
 * */

static void LED2_Task(void* parameter)
{
  while(1)
  {
    printf("LED2_ON!\r\n");
    LED2_ON;
    vTaskDelay(1000);/*��ʱ500��tick*/
    printf("LED2_OFF!\r\n");
    LED2_OFF;
    vTaskDelay(1000);
  }
}

/**
 * @brief Ϊ�˷������һ���������񴴽���������������
 * 
 */
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;//����һ��������Ϣ����ֵ��
  taskENTER_CRITICAL();//�����ٽ���
  /*����LED1_Task����*/
  xReturn=xTaskCreate((TaskFunction_t )LED1_Task,//������
                      (const char*)"LED1_Task",//
                      (uint32_t   )512,//
                      (void*      )NULL,//���ݸ�����Ĳ���
                      (UBaseType_t)2,//�������ȼ�
                      (TaskHandle_t*)&LED1_Task_Handle);//������ƿ�
  if(pdPASS==xReturn)
    printf("LED1_Task���񴴽��ɹ�\r\n");
  else
    printf("LED1_Task���񴴽�ʧ��\r\n");
    /*����LED2_Task����*/
  xReturn=xTaskCreate((TaskFunction_t )LED2_Task,//������
                      (const char*)"LED2_Task",//
                      (uint32_t   )512,//
                      (void*      )NULL,//���ݸ�����Ĳ���
                      (UBaseType_t)3,//�������ȼ�
                      (TaskHandle_t*)&LED2_Task_Handle);//������ƿ�
  if(pdPASS==xReturn)
    printf("LED2_Task���񴴽��ɹ�\r\n");
  else
    printf("LED2_Task���񴴽�ʧ��\r\n");


  vTaskDelete(AppTaskCreate_Handle);//ɾ��AppTaskCreate_Handle����
  taskEXIT_CRITICAL();//�˳��ٽ���

}
int main(void)
{	
  BaseType_t xReturn =pdPASS; //����һ��������Ϣ�ķ���ֵ��Ĭ��ΪpdPASS
  BSP_init();/*������Ӳ����ʼ��*/
  printf("����һ������STM32F103��FreeRTOSϵͳ�Ķ�̬�����񴴽��Ĳ���!\r\n");
  /*����AppTaskCreate����*/
 xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate,
                                      (const char*   )"AppTaskCreate",//��������
                                      (uint32_t      )512,//�����ջ��С
                                      (void*         )NULL,//���ݸ��������Ĳ���
                                      (UBaseType_t   )1,//�������ȼ�
                                      (TaskHandle_t*  )&AppTaskCreate_Handle);  
if(NULL!=xReturn)/*�����ɹ�*/
    vTaskStartScheduler();/*�������񣬿�������*/
else
   return 1;


while(1);
}

/********************************END OF FILE****************************/
