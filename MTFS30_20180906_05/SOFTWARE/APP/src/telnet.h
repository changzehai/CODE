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

/* 客户端连接信息结构定义 */
typedef struct _client_t_ {
	//tcp server 创建的新连接的netconn
	struct netconn *newconn;
	//子任务的TCB，采用内存管理的方式控制申请	
	OS_TCB *Client_TaskTCB;	
	//任务堆栈，采用内存管理的方式控制申请	
	CPU_STK *Client_TASK_STK;	    
} client_t;

/* 鍑芥暟澹版槑 */

void Echo_Server_Task(void);

#endif