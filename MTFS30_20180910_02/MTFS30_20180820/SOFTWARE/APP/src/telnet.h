#ifndef __TCP_SERVER_H_
#define __TCP_SERVER_H_


#ifndef CLIENT_TASK_MAX
#define CLIENT_TASK_MAX		MEMP_NUM_NETCONN - 1		
#endif


#ifndef CLIENT_TASK_PRIO_BASE
#define CLIENT_TASK_PRIO_BASE		6
#endif


#ifndef CLIENT_TASK_STACK_SIZE
#define CLIENT_TASK_STACK_SIZE		256
#endif

/* �ͻ���������Ϣ�ṹ���� */
typedef struct _client_t_ {
	//tcp server �����������ӵ�netconn
	struct netconn *newconn;
	//�������TCB�������ڴ����ķ�ʽ��������	
	OS_TCB *Client_TaskTCB;	
	//�����ջ�������ڴ����ķ�ʽ��������	
	CPU_STK *Client_TASK_STK;	    
} client_t;

/* 函数声明 */

void Echo_Server_Task(void);

#endif