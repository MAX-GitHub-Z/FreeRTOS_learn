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
#include "queue.h"

/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"


/*����C����ͷ�ļ�*/
#include <stdio.h>
#include <string.h>
/**************************** ������ ********************************/
 /*
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������Ϊ NULL��
 */
/*����������*/
static TaskHandle_t AppTaskCreate_Handle=NULL;
/*������Ϣ���*/
static TaskHandle_t Receive_Task_Handle = NULL;
/*������Ϣ���*/
static TaskHandle_t Send_Task_Handle = NULL;


/***************************** �ں˶����� *****************************/
 /*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 *
 �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 *
 */


QueueHandle_t Test_Queue =NULL;



 /*************************** �궨�� ************************************/
 /*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩ�궨�塣
 */
 #define QUEUE_LEN 4 /* ���еĳ��ȣ����ɰ������ٸ���Ϣ */
 #define QUEUE_SIZE 4 /* ������ÿ����Ϣ��С���ֽڣ� */
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
 Key_GPIO_Config(); //�����˿ڳ�ʼ��
 USART_Config(); //USART���ڳ�ʼ��
}
/*
***********************************************************
 * ������Ϣ����ӡ
 * ********************************************************
 * */

static void Receive_Task(void* parameter)
{
  BaseType_t xReturn = pdTRUE; //����һ��������Ϣ����ֵ��Ĭ��ΪpdTRUE
  uint32_t r_queue;//����һ��������Ϣ�ı���
  while(1)
  {
    xReturn =xQueueReceive(Test_Queue,//��Ϣ���о��
                          &r_queue,//������Ϣ����
                          portMAX_DELAY);//�ȴ�ʱ�䣬һֱ��
    if(pdTRUE==xReturn)
      printf("���ν��յ���������%d\n",r_queue);
    else
      printf("���ݽ��ճ���,�������: 0x%lx\n",xReturn);
  }
}

/********************************
 * ����һ��������������������������
 * ���°���K1������LED1����LED2�ָ�
 * ���°���K2������LED2����LED1�ָ�
 * 
 ********************************
 * */
static void Send_Task(void* parameter)
{
  BaseType_t xReturn=pdPASS;//����һ��������Ϣ����ֵ��Ĭ��ΪpaASS
  uint32_t send_data1=1;
  uint32_t send_data2=2;
  while (1)
  {
    if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) ==KEY_ON)
    {
      /*K1����������*/
      printf("������Ϣsend_data1\r\n");
      xReturn=xQueueSend(Test_Queue,//��Ϣ���о�� 
                       &send_data1,//������Ϣ����
                        0 );//�ȴ�ʱ��0
			if(pdPASS == xReturn)
        printf("��Ϣsend_data1���ͳɹ�!\n\n");
    }
    if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN)==KEY_ON)
    {
      /*K2����������*/
      
     printf("������Ϣsend_data2\r\n");
      xReturn=xQueueSend(Test_Queue,//��Ϣ���о�� 
                       &send_data2,//������Ϣ����
                        0 );//�ȴ�ʱ��0
			if(pdPASS == xReturn)
        printf("��Ϣsend_data2���ͳɹ�!\n\n");
    }
    vTaskDelay(20);//��ʱ20��tick
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
  /*������Ϣ����Test_Queue*/
  Test_Queue=xQueueCreate((UBaseType_t)QUEUE_LEN,//��Ϣ���еĳ���
                          (UBaseType_t)QUEUE_SIZE);//��Ϣ�Ĵ�С
  /*����Receive_Task����*/
  xReturn=xTaskCreate((TaskFunction_t )Receive_Task,//������
                      (const char*)"Receive_Task",//
                      (uint32_t   )512,//
                      (void*      )NULL,//���ݸ�����Ĳ���
                      (UBaseType_t)2,//�������ȼ�
                      (TaskHandle_t*)&Receive_Task_Handle);//������ƿ�
  if(pdPASS==xReturn)
    printf("Receive_Task���񴴽��ɹ�\r\n");
  else
    printf("Receive_Task���񴴽�ʧ��\r\n");
    /*����Send_Task����*/
  xReturn=xTaskCreate((TaskFunction_t )Send_Task,//������
                      (const char*)"Send_Task",//
                      (uint32_t   )512,//
                      (void*      )NULL,//���ݸ�����Ĳ���
                      (UBaseType_t)3,//�������ȼ�
                      (TaskHandle_t*)&Send_Task_Handle);//������ƿ�
  if(pdPASS==xReturn)
    printf("Send_Task���񴴽��ɹ�\r\n");
  else
    printf("Send_Task���񴴽�ʧ��\r\n");


  vTaskDelete(AppTaskCreate_Handle);//ɾ��AppTaskCreate_Handle����
  taskEXIT_CRITICAL();//�˳��ٽ���

}
int main(void)
{	
  BaseType_t xReturn =pdPASS; //����һ��������Ϣ�ķ���ֵ��Ĭ��ΪpdPASS
  BSP_init();/*������Ӳ����ʼ��*/
  printf("����һ������STM32F103��FreeRTOSϵͳ����Ϣ���еĲ���!\r\n");
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
