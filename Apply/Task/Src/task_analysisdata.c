#include "task_conf.h"
#include "usercode.h"
#include "config.h"

static char AnalysisData[5][15];
static int AnalysisNum = 0;

/* 分析从上位机处接收到的数据 */
void Task_AnalysisData(uint8_t *DataBuf)
{
    //将变量清0
    memset(AnalysisData,0,sizeof(AnalysisData));
    HandleModeInfo HMInfo = {" ",0,0,0};
    AutoModeInfo AMInfo = {" ",0};
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
            HMInfo.fNum[0] = strtof(AnalysisData[2],NULL);
            HMInfo.fNum[1] = strtof(AnalysisData[3],NULL);
            HMInfo.fNum[2] = strtof(AnalysisData[4],NULL);

            //更新深度环PID
            Algo_PID_Update(&DepthPID,HMInfo.fNum);
            printf("DepthPID %f %f %f\r\n",DepthPID.fKp,DepthPID.fKi,DepthPID.fKd);
        }
        //艏向环PID
        else if(!strcmp(AnalysisData[1],"YawPID"))
        {
            HMInfo.fNum[0] = strtof(AnalysisData[2],NULL);
            HMInfo.fNum[1] = strtof(AnalysisData[3],NULL);
            HMInfo.fNum[2] = strtof(AnalysisData[4],NULL);

            //更新艏向环PID
            Algo_PID_Update(&YawPID,HMInfo.fNum);
            printf("YawPID %f %f %f\r\n",YawPID.fKp,YawPID.fKi,YawPID.fKd);
        }
        //巡线环PID
        else if(!strcmp(AnalysisData[1],"LinePatrolPID"))
        {
            HMInfo.fNum[0] = strtof(AnalysisData[2],NULL);
            HMInfo.fNum[1] = strtof(AnalysisData[3],NULL);
            HMInfo.fNum[2] = strtof(AnalysisData[4],NULL);
            
            //更新巡线环PID
            Algo_PID_Update(&LinePatrolPID,HMInfo.fNum);
            printf("LinePatrolPID %f %f %f\r\n",LinePatrolPID.fKp,LinePatrolPID.fKi,LinePatrolPID.fKd);
        }
    }
    //手柄摇杆值
    else if(!strcmp(AnalysisData[0],"JSV"))
    {
        HMInfo.fNum[0] = strtof(AnalysisData[1],NULL);
        HMInfo.fNum[1] = strtof(AnalysisData[2],NULL);
        //printf("%f %f\r\n",HMInfo.fNum[0],HMInfo.fNum[1]);
        //printf("%x\r\n",fNum);

        //将数据发送给手柄控制线程
        rt_mq_send(HandleModemq,&HMInfo,sizeof(HandleModeInfo));
    }
    //手柄按键状态
    else if(!strcmp(AnalysisData[0],"JSB"))
    {
        HMInfo.fNum[0] = strtof(AnalysisData[1],NULL);
        if(!strcmp(AnalysisData[2],"Press"))
        {
            HMInfo.fNum[1] = 1; //用1代表Press
            //printf("%d %d\r\n",(uint8_t)HMInfo.fNum[0],1);
        }
        else
        {
            HMInfo.fNum[1] = 0; //用0代表Release
            //printf("%d %d\r\n",(uint8_t)HMInfo.fNum[0],0);
        }
        //将数据发送给手柄控制线程
        rt_mq_send(HandleModemq,&HMInfo,sizeof(HandleModeInfo));
    }

    //模式切换命令
    else if(!strcmp(AnalysisData[0],"MODE"))
    {
        //printf("%s\r\n",AnalysisData[1]);
        if(!strcmp(AnalysisData[1],"AUTO"))
        {
            //挂起手柄控制模式，启动自动巡线模式
            memcpy(HMInfo.ModeChange,"AUTO START",sizeof("AUTO START"));
            rt_mq_send(HandleModemq,&HMInfo,sizeof(HandleModeInfo));
        }
        else if(!strcmp(AnalysisData[1],"HANDLE"))
        {
            //挂起自动巡线模式，启动手柄模式
            memcpy(AMInfo.ModeChange,"HANDLE START",sizeof("HANDLE START"));
            rt_mq_send(AutoModemq,&AMInfo,sizeof(AutoModeInfo));              
        }
    }

    //自动巡线模式角度值
    else if(!strcmp(AnalysisData[0],"LP"))
    {
        AMInfo.BlackAngle = strtof(AnalysisData[1],NULL);
        rt_mq_send(AutoModemq,&AMInfo,sizeof(AutoModeInfo));
    }
}

