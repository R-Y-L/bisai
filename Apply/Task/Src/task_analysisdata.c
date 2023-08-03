#include "task_conf.h"
#include "usercode.h"
#include "config.h"

static char AnalysisData[5][15];
static int AnalysisNum = 0;
float fNum[4];

/* 分析从上位机处接收到的数据 */
void Task_AnalysisData(uint8_t *DataBuf)
{
    //将变量清0
    memset(AnalysisData,0,sizeof(AnalysisData));
    memset(fNum,0,4);
    AnalysisNum = 0;

    //根据格式切分字符串存放到二维数组中
    AnalysisNum = sscanf((char *)DataBuf,"%s %s %s %s %s",
                        AnalysisData[0],
                        AnalysisData[1],
                        AnalysisData[2],
                        AnalysisData[3],
                        AnalysisData[4]);
    //printf("%d ,%s,%s,%s,%s,%s\r\n",AnalysisNum,AnalysisData[0],AnalysisData[1],AnalysisData[2],AnalysisData[3],AnalysisData[4]);

    if(!AnalysisNum) return;
    /* 根据帧头选择任务 */
    //PID值
    if(!strcmp(AnalysisData[0],"PID"))
    {
        //深度环PID
        if(!strcmp(AnalysisData[1],"DepthPID"))
        {
            fNum[0] = strtof(AnalysisData[2],NULL);
            fNum[1] = strtof(AnalysisData[3],NULL);
            fNum[2] = strtof(AnalysisData[4],NULL);
            printf("%f %f %f\r\n",fNum[0],fNum[1],fNum[2]);

            //更新深度环PID
        }
        //艏向环PID
        else if(!strcmp(AnalysisData[1],"YawPID"))
        {
            fNum[0] = strtof(AnalysisData[2],NULL);
            fNum[1] = strtof(AnalysisData[3],NULL);
            fNum[2] = strtof(AnalysisData[4],NULL);
            printf("%f %f %f\r\n",fNum[0],fNum[1],fNum[2]);

            //更新艏向环PID
        }
        //巡线环PID
        else if(!strcmp(AnalysisData[1],"LinePatrolPID"))
        {
            fNum[0] = strtof(AnalysisData[2],NULL);
            fNum[1] = strtof(AnalysisData[3],NULL);
            fNum[2] = strtof(AnalysisData[4],NULL);
            printf("%f %f %f\r\n",fNum[0],fNum[1],fNum[2]);

            //更新巡线环PID
        }
    }
    //手柄摇杆值
    else if(!strcmp(AnalysisData[0],"JSV"))
    {
        fNum[0] = strtof(AnalysisData[1],NULL);
        fNum[1] = strtof(AnalysisData[2],NULL);
        //printf("%f %f\r\n",fNum[0],fNum[1]);
        //printf("%x\r\n",fNum);
        //将数据发送给手柄控制线程
        //rt_mq_send(HandleModemq,fNum,sizeof(fNum));
        rt_sem_release(JSData_Sem);
    }
    //手柄按键状态
    else if(!strcmp(AnalysisData[0],"JSB"))
    {
        fNum[0] = strtof(AnalysisData[1],NULL);
        if(!strcmp(AnalysisData[2],"Press"))
        {
            fNum[1] = 1;
            //printf("%d %d\r\n",(uint8_t)fNum[0],1);
        }
        else
        {
            fNum[1] = 0;
            //printf("%d %d\r\n",(uint8_t)fNum[0],0);
        }
        //将数据发送给手柄控制线程
        rt_sem_release(JSData_Sem);
    }

    //键盘鼠标控制命令
    else if(!strcmp(AnalysisData[0],"C"))
    {
        printf("%s\r\n",AnalysisData[1]);

        //将数据发送给手柄控制线程
    }

    //模式切换命令
    else if(!strcmp(AnalysisData[0],"MODE"))
    {
        //printf("%s\r\n",AnalysisData[1]);
        if(!strcmp(AnalysisData[1],"AUTO"))
        {
            //挂起手柄控制模式，启动自动巡线模式
            rt_mq_send(HandleModemq,"AUTO START",sizeof("AUTO START"));
        }
        else if(!strcmp(AnalysisData[1],"HANDLE"))
        {
            //挂起自动巡线模式，启动手柄模式
            rt_mq_send(AutoModemq,"HANDLE START",sizeof("HANDLE START"));
        }
    }
}