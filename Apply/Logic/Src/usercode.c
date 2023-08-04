#include "usercode.h"		/* usercode头文件 */
#include "drv_hal_conf.h"   /* SGA库头文件配置 */
#include "task_conf.h"      /* task层头文件配置 */
#include "ocd_conf.h"       /* OCD层头文件配置 */
#include "dev_conf.h"		/* Dev层头文件配置 */
#include "algo_conf.h"		/* Algo层头文件配置 */
#include "config.h"			/* I/O配置头文件配置 */

/* 线程句柄 */
rt_thread_t thread1 = RT_NULL;
rt_thread_t thread2 = RT_NULL;
rt_thread_t thread3 = RT_NULL;
rt_thread_t thread4 = RT_NULL;
rt_thread_t thread5 = RT_NULL;
rt_thread_t thread6 = RT_NULL;

/* 信号量句柄*/
rt_sem_t DataFromIPC_Sem = RT_NULL;	//上位机数据信号量
rt_sem_t JY901S_Sem = RT_NULL;		//JY901S数据信号量

/*消息队列句柄*/
rt_mq_t AutoModemq = RT_NULL;	//自动模式消息队列
rt_mq_t HandleModemq = RT_NULL;	//手动模式消息队列

ALIGN(RT_ALIGN_SIZE)
/* 用户逻辑代码 */
void UserLogic_Code(void)
{
	/* 创建信号量 */
	JY901S_Sem = rt_sem_create("JY901Sem",0,RT_IPC_FLAG_FIFO);
	//if(RT_NULL != JY901S_Sem)	printf("JY901Sem create successful\r\n");

	DataFromIPC_Sem = rt_sem_create("DataFromIPC_Sem",0,RT_IPC_FLAG_FIFO);
	//if(RT_NULL != DataFromIPC_Sem)	printf("DataFromIPC_Sem create successful\r\n");

	/* 创建消息队列 */
	AutoModemq = rt_mq_create("AutoModemq",50,10,RT_IPC_FLAG_FIFO);
	//if(RT_NULL != AutoModemq)		printf("AutoModemq create successful\r\n");
	HandleModemq = rt_mq_create("HandleModemq",50,10,RT_IPC_FLAG_FIFO);
	//if(RT_NULL != HandleModemq)		printf("HandleModemq create successful\r\n");

	/* 
		线程1 上位机数据读取
		线程2 JY901S数据读取
		线程3 MS5837数据读取
		线程4 手柄运动模式
		线程5 自动巡线模式
		线程6 测试线程
	 */

	/* 创建线程 */
	thread1 = rt_thread_create("DataFromIPC",DataFromIPC,NULL,1024,1,20);
	// if(RT_NULL != thread1)
	// 	printf("RT-Thread create thread1 successful\r\n");

	thread2 = rt_thread_create("JY901SReadThread",JY901SReadThread,NULL,512,4,20);
	// if(RT_NULL != thread2)
	// 	printf("JY901SReadThread create successful\r\n");


	thread3 = rt_thread_create("MS5837ReadThread",MS5837ReadThread,NULL,512,4,20);
	// if(RT_NULL != thread3)
	// 	printf("MS5837ReadThread create successful\r\n");	


	thread4 = rt_thread_create("MODE_HANDLE",MODE_HANDLE,NULL,512,2,40);
	// if(RT_NULL != thread4)
	// 	printf("MODE_HANDLE create successful\r\n");	
	

	thread5 = rt_thread_create("MODE_AUTO",MODE_AUTO,NULL,512,2,40);
	// if(RT_NULL != thread5)
	// 	printf("MODE_AUTO create successful\r\n");

	thread6 = rt_thread_create("TestThread",TestThread,NULL,512,8,20);
	// if(RT_NULL != thread6)
	// 	printf("TestThread create successful\r\n");
	

	// if(rt_thread_startup(thread1) == RT_EOK) printf("DataFromIPC start\r\n");
	// if(rt_thread_startup(thread2) == RT_EOK) printf("JY901SReadThread start\r\n");
	// if(rt_thread_startup(thread3) == RT_EOK) printf("MS5837ReadThread start\r\n");
	// if(rt_thread_startup(thread4) == RT_EOK) printf("MODE_HANDLE start\r\n");
	// if(rt_thread_startup(thread5) == RT_EOK) printf("MODE_AUTO start\r\n");	//挂起自动巡线模式,等待上位机发送命令以开启
	// if(rt_thread_startup(thread6) == RT_EOK) printf("TestThread start\r\n");

	rt_thread_startup(thread1);
	rt_thread_startup(thread2);
	rt_thread_startup(thread3);
	rt_thread_startup(thread4);
	rt_thread_startup(thread5);
	rt_thread_startup(thread6);

}
