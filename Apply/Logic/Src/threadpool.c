#include "usercode.h"		/* usercode头文件 */
#include "threadpool.h"		/* threadpool头文件 */
#include "drv_hal_conf.h"   /* SGA库头文件配置 */
#include "task_conf.h"      /* task层头文件配置 */
#include "ocd_conf.h"       /* OCD层头文件配置 */
#include "dev_conf.h"		/* Dev层头文件配置 */
#include "algo_conf.h"		/* Algo层头文件配置 */
#include "config.h"			/* I/O配置头文件配置 */

//存储所有推进器的数据
PWMInfo_T PWMInfo = {1500,1500,1500,1500};

/* 线程入口函数（使用裸机忽略此文件） */
/* 读取上位机数据线程 */
void DataFromIPC(void* paramenter)
{
    uint8_t ReceBuf[100];
    uint8_t ReceNum = 0;

    while(1)
    {
        if(rt_sem_take(DataFromIPC_Sem,RT_WAITING_FOREVER) == RT_EOK)
        {
            //平时调试使用串口1，正式使用采用串口3,修改PRINTF_UART宏即可
            if(PRINTF_UART == USART1)
                ReceNum = Drv_Uart_Receive_DMA(&Uart1,ReceBuf);
            else if(PRINTF_UART == USART3)
                ReceNum = Drv_Uart_Receive_DMA(&Uart3,ReceBuf);
            
            if(ReceNum != 0)
            {
                //printf("%s\r\n",ReceBuf);
                //分析数据
                Task_AnalysisData(ReceBuf);

                //将临时变量清0
                rt_memset(ReceBuf,0,ReceNum);
                ReceNum = 0;
            }
        }
        Drv_Delay_Ms(1);    //让出CPU资源给低优先级线程
    }
}

/* 读取JY901S数据线程 */
void JY901SReadThread(void* paramenter)
{
    while(1)
    {
        //如果获取到信号量，说明接收到数据
		if(rt_sem_take(JY901S_Sem,RT_WAITING_FOREVER) == RT_EOK)
		{
            //如果成功处理完成数据
			if(OCD_JY901_DataProcess(&JY901S))
            {
                //数据转换
                OCD_JY901_DataConversion(&JY901S);
                //打印欧拉角
                if(JY901S.tConfig.usType & JY901_OUTPUT_ANGLE)	    
                    printf("J Angle %.3f %.3f %.3f\r\n",
                            JY901S.stcAngle.ConRoll,
                            JY901S.stcAngle.ConPitch,
                            JY901S.stcAngle.ConYaw);
            }
		}
        Drv_Delay_Ms(1);    //让出CPU资源给低优先级线程
		rt_thread_yield();
    }
}

/* 读取MS5837数据线程 */
void MS5837ReadThread(void* paramenter)
{
    while(1)
    {
        OCD_MS5837_GetData(&MS5837);
        if(MS5837.fDepth == 153150.250000)  //未接MS5837的错误数据
            MS5837.fDepth = 0;
        printf("M %0.2f\r\n",MS5837.fDepth);
        Drv_Delay_Ms(600);
    }
}

/* 手柄控制线程 */
void HANDLE_MODE(void* paramenter)
{
    HandleModeInfo HMInfo;
    AutoModeInfo ClearBuf;

    while(1)
    {
        //手柄控制消息队列接收到数据，将切换到自动模式
        if(rt_mq_recv(HandleModemq,&HMInfo,sizeof(HandleModeInfo),RT_WAITING_NO) == RT_EOK)
        {
            if(!rt_strcmp(HMInfo.ModeChange,"AUTO START"))
            {
                //将AutoModemq消息队列中所有内容清空
                while(1)
                {
                    if(rt_mq_recv(AutoModemq,&ClearBuf,sizeof(AutoModeInfo),RT_WAITING_NO) != RT_EOK)
                        break;
                }
                Task_Thruster_AllStop();                //所有推进器停转
                
                printf("Switch to AUTO Mode\r\n");
                rt_enter_critical();                    //调度器上锁
                rt_thread_suspend(rt_thread_self());    //挂起本线程
                rt_thread_resume(thread5);              //恢复自动控制线程
                rt_exit_critical();                     //调度器解锁
                rt_schedule();                          //立即执行一次调度
            }
            else
            {
                Task_HandleMode_Process(HMInfo);    //手柄控制模式处理函数
                Drv_Delay_Ms(500);  //执行时间与上位机手柄发送一帧数据时间相同
                Task_Thruster_Stop(LeftHThruster);
                Task_Thruster_Stop(RightHThruster);
            }
        }
        // printf("HANDLE\r\n");

        Drv_Delay_Ms(1);    //让出CPU资源给低优先级线程
    }
}

/* 巡线模式线程 */
void AUTO_MODE(void* paramenter)
{
    AutoModeInfo AMInfo;
    HandleModeInfo ClearBuf;

    //默认挂起自动模式
    rt_thread_suspend(rt_thread_self());
    rt_schedule();
    while(1)
    {
        //自动控制消息队列接收到数据，将切换到手柄模式
        if(rt_mq_recv(AutoModemq,&AMInfo,sizeof(AutoModeInfo),RT_WAITING_NO) == RT_EOK)
        {
            if(!rt_strcmp(AMInfo.ModeChange,"HANDLE START"))
            {
                //将HandleModemq队列中所有内容清空
                while(1)
                {
                    if(rt_mq_recv(HandleModemq,&ClearBuf,sizeof(HandleModeInfo),RT_WAITING_NO) != RT_EOK)
                        break;
                }
                Task_Thruster_AllStop();                //所有推进器停转

                printf("Switch to HANDLE Mode\r\n");
                rt_enter_critical();                    //调度器上锁
                rt_thread_suspend(rt_thread_self());    //挂起本线程
                rt_thread_resume(thread4);              //恢复手动控制线程
                rt_exit_critical();                     //调度器解锁
                rt_schedule();                          //立即执行一次调度
            }
            else
            {
                //自动模式处理函数，根据消息队列中传来的黑线角度改变推进器PWM
                Task_AutoMode_Process(AMInfo);
                //printf("%f\r\n",AMInfo.BlackAngle);
                Drv_Delay_Ms(300);
                Task_Thruster_Stop(LeftHThruster);
                Task_Thruster_Stop(RightHThruster);
            }
        }

        // printf("AUTO\r\n");
        // Drv_Delay_Ms(1000);
        Drv_Delay_Ms(1);    //让出CPU资源给低优先级线程
    }
}

/* 定深控制 */
void DepthControl(void* paramenter)
{
    DepthControlInfo DCInfo;
    float ExpDepth = 0.0f;
    float CurrDepth = 0.0f;

    while(1)
    {
        //定深数据消息队列接收到数据，将开始定深控制
        if(rt_mq_recv(DepthControlmq,&DCInfo,sizeof(DepthControlInfo),RT_WAITING_NO) == RT_EOK)
        {
            ExpDepth = DCInfo.setDepth;
            //printf("%f",ExpDepth);
        }

        //获取当前深度
        CurrDepth = MS5837.fDepth;
        //定深控制函数
        task_DepthControl_Process(CurrDepth,ExpDepth);

        Drv_Delay_Ms(600);    //每隔一段时间进行一次定深
    }
}

/* 汇报PWMout值 */
void ReportPWMout(void* paramenter)
{
    while(1)
    {
        printf("T %d %d %d %d\r\n",
                    PWMInfo.PWMout[LeftHThruster],
                    PWMInfo.PWMout[RightHThruster],
                    PWMInfo.PWMout[LeftVThruster],
                    PWMInfo.PWMout[RightVThruster]);
        Drv_Delay_Ms(1000);    //每隔一段时间进行一次汇报
    }
}

/* 测试线程 */
void TestThread(void* paramenter)
{
    while(1)
    {
        
        Task_Thruster_AllSpeedSet(1500);
        //printf("test\r\n");
        Drv_Delay_Ms(1000);
        
        Task_Thruster_AllSpeedSet(1550);
        
        Drv_Delay_Ms(1000);
    }
    //超过1500为前进 0号为左侧水平推进器
    //超过1500为上浮 1号为左侧垂直推进器
    //超过1500为上浮 2号为右侧垂直推进器
    //超过1500为前进 3号为右侧水平推进器
}



