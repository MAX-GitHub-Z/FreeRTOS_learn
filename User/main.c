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
#include "semphr.h"//��ֵ�ź���ͷ�ļ�

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
/*�������ݽ��մ�����*/
static TaskHandle_t USART_Task_Handle=NULL;
/*������Ϣ���*/
static TaskHandle_t KEY2_Task_Handle = NULL;
/*������Ϣ���*/
static TaskHandle_t KEY1_Task_Handle = NULL;


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


//QueueHandle_t Test_Queue =NULL;
SemaphoreHandle_t BinarySem_Handle =NULL;//��ֵ�ź������


 /*************************** �궨�� ************************************/
 /*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩ�궨�塣
 */
 #define QUEUE_LEN 1 /* ���еĳ��ȣ����ɰ������ٸ���Ϣ */
 #define QUEUE_SIZE 4 /* ������ÿ����Ϣ��С���ֽڣ� */




char Usart_Rx_Buf[USART_RBUFF_SIZE];//���ڽ�����������


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
/**
 * ���ڴ������
 * 
 */
static void USART_Task(void* parameter)
{
  char* data=Usart_Rx_Buf;
  while (1)
  {
    //printf("����%s:",data);
    /* code */
  }
  

}



/*
***********************************************************
 * ����KEY1��ȡ��ֵ�ź���
 * ********************************************************
 * */

static void KEY1_Task(void* parameter)
{
  BaseType_t xSemaphore=NULL;
  while(1)
  {
    if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )
    {
      xSemaphore=xSemaphoreTake(BinarySem_Handle,0);//��ȡ��ֵ�ź���
      if(xSemaphore==pdPASS)
        printf("��ȡ��ֵ�ź����ɹ�\r\n");
      else
        printf("��ȡ��ֵ�ź���ʧ��\r\n");
    }
    vTaskDelay(10);
  }
}

/********************************
 * ����KEY2�ͷŶ�ֵ�ź���
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
      xSemaphore=xSemaphoreGive(BinarySem_Handle);//�ͷŶ�ֵ�ź���
      if(xSemaphore==pdTRUE)
        printf("�ͷŶ�ֵ�ź����ɹ�\r\n");
      else
        printf("�ͷŶ�ֵ�ź���ʧ��\r\n");
    }
    vTaskDelay(100);
  }
}

/**
 * @brief Ϊ�˷������һ���������񴴽���������������
 * 
 */
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;//����һ��������Ϣ����ֵ
  
  taskENTER_CRITICAL();//�����ٽ���
  BinarySem_Handle = xSemaphoreCreateBinary();
  if(BinarySem_Handle == NULL)
      printf("��ֵ�ź�������ʧ��\r\n");
  else
      printf("��ֵ�ź��������ɹ�\r\n");

  /*����KEY1_Task����*/
  xReturn=xTaskCreate((TaskFunction_t )KEY1_Task,//������
                      (const char*)"KEY1_Task",//
                      (uint32_t   )512,//
                      (void*      )NULL,//���ݸ�����Ĳ���
                      (UBaseType_t)2,//�������ȼ�
                      (TaskHandle_t*)&KEY1_Task_Handle);//������ƿ�
  if(pdPASS==xReturn)
    printf("KEY1_Task���񴴽��ɹ�\r\n");
  else
    printf("KEY1_Task���񴴽�ʧ��\r\n");
    /*����KEY2_Task����*/
  xReturn=xTaskCreate((TaskFunction_t )KEY2_Task,//������
                      (const char*)"KEY2_Task",//
                      (uint32_t   )512,//
                      (void*      )NULL,//���ݸ�����Ĳ���
                      (UBaseType_t)3,//�������ȼ�
                      (TaskHandle_t*)&KEY2_Task_Handle);//������ƿ�
  if(pdPASS==xReturn)
    printf("KEY2_Task���񴴽��ɹ�\r\n");
  else
    printf("KEY2_Task���񴴽�ʧ��\r\n");

 /*����USART_Task����*/
  xReturn=xTaskCreate((TaskFunction_t )USART_Task,//������
                      (const char*)"USART_Task",//
                      (uint32_t   )512,//
                      (void*      )NULL,//���ݸ�����Ĳ���
                      (UBaseType_t)1,//�������ȼ�
                      (TaskHandle_t*)&USART_Task_Handle);//������ƿ�
  if(pdPASS==xReturn)
    printf("USART_Task���񴴽��ɹ�\r\n");
  else
    printf("USART_Task���񴴽�ʧ��\r\n");

  vTaskDelete(AppTaskCreate_Handle);//ɾ��AppTaskCreate_Handle����
  taskEXIT_CRITICAL();//�˳��ٽ���

}
int main(void)
{	
  BaseType_t xReturn =pdPASS; //����һ��������Ϣ�ķ���ֵ��Ĭ��ΪpdPASS
  BSP_init();/*������Ӳ����ʼ��*/
  printf("����һ������STM32F103��FreeRTOSϵͳ�Ķ�ֵ�ź����Ĳ���!\r\n");
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
