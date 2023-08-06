#include "task_conf.h"
#include "usercode.h"
#include "config.h"

static char AnalysisData[5][15];
static int AnalysisNum = 0;

/* 分析从上位机处接收到的数据 */
void Task_AnalysisData(uint8_t *DataBuf)
{
    //将变量清0
    rt_memset(AnalysisData,0,sizeof(AnalysisData));
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

    //手柄摇杆值
    else if(!rt_strcmp(AnalysisData[0],"JSV"))
    {
        HMInfo.fNum[0] = strtof(AnalysisData[1],NULL);
        HMInfo.fNum[1] = strtof(AnalysisData[2],NULL);
        //printf("%f %f\r\n",HMInfo.fNum[0],HMInfo.fNum[1]);
        //printf("%x\r\n",fNum);

        //将数据发送给手柄控制线程
        rt_mq_send(HandleModemq,&HMInfo,sizeof(HandleModeInfo));
    }

    //手柄按键状态
    else if(!rt_strcmp(AnalysisData[0],"JSB"))
    {
        uint8_t key = strtod(AnalysisData[1],NULL);

        if(!rt_strcmp(AnalysisData[2],"Press"))
        {
            HMInfo.keyPressed = true;//用1代表Press
        }
        else
        {
            HMInfo.keyPressed = false; //用0代表Release
        }

        switch(key)
        {
            //后退
            case 0: 
                HMInfo.fNum[0] = 180.0f;
                HMInfo.fNum[1] = 100.0f;
                break;
            //右转
            case 1:
                HMInfo.fNum[0] = 90.0f;
                HMInfo.fNum[1] = 100.0f;
                break;
            //左转
            case 2:
                HMInfo.fNum[0] = -90.0f;
                HMInfo.fNum[1] = 100.0f;
                break;
            //前进
            case 3:
                HMInfo.fNum[0] = 0.0f;
                HMInfo.fNum[1] = 100.0f;
                break;
            default:;
        }
        //将数据发送给手柄控制线程
        rt_mq_send(HandleModemq,&HMInfo,sizeof(HandleModeInfo));
    }

    //自动巡线模式角度值
    else if(!rt_strcmp(AnalysisData[0],"LP"))
    {
        AMInfo.BlackAngle = strtof(AnalysisData[1],NULL);
        rt_mq_send(AutoModemq,&AMInfo,sizeof(AutoModeInfo));
    }

    //模式切换命令
    else if(!rt_strcmp(AnalysisData[0],"MODE"))
    {
        //printf("%s\r\n",AnalysisData[1]);
        if(!rt_strcmp(AnalysisData[1],"AUTO"))
        {
            //挂起手柄控制模式，启动自动巡线模式
            rt_memcpy(HMInfo.ModeChange,"AUTO START",sizeof("AUTO START"));
            rt_mq_send(HandleModemq,&HMInfo,sizeof(HandleModeInfo));
        }
        else if(!rt_strcmp(AnalysisData[1],"HANDLE"))
        {
            //挂起自动巡线模式，启动手柄模式
            rt_memcpy(AMInfo.ModeChange,"HANDLE START",sizeof("HANDLE START"));
            rt_mq_send(AutoModemq,&AMInfo,sizeof(AutoModeInfo));              
        }
    }

    //PID值
    else if(!rt_strcmp(AnalysisData[0],"PID"))
    {
        //深度环PID
        if(!rt_strcmp(AnalysisData[1],"DepthPID"))
        {
            HMInfo.fNum[0] = strtof(AnalysisData[2],NULL);
            HMInfo.fNum[1] = strtof(AnalysisData[3],NULL);
            HMInfo.fNum[2] = strtof(AnalysisData[4],NULL);

            //更新深度环PID
            Algo_PID_Update(&DepthPID,HMInfo.fNum);
            printf("DepthPID %.2f %.2f %.2f\r\n",DepthPID.fKp,DepthPID.fKi,DepthPID.fKd);
        }
        //艏向环PID
        else if(!rt_strcmp(AnalysisData[1],"YawPID"))
        {
            HMInfo.fNum[0] = strtof(AnalysisData[2],NULL);
            HMInfo.fNum[1] = strtof(AnalysisData[3],NULL);
            HMInfo.fNum[2] = strtof(AnalysisData[4],NULL);

            //更新艏向环PID
            Algo_PID_Update(&YawPID,HMInfo.fNum);
            printf("YawPID %.2f %.2f %.2f\r\n",YawPID.fKp,YawPID.fKi,YawPID.fKd);
        }
        //巡线环PID
        else if(!rt_strcmp(AnalysisData[1],"LinePatrolPID"))
        {
            HMInfo.fNum[0] = strtof(AnalysisData[2],NULL);
            HMInfo.fNum[1] = strtof(AnalysisData[3],NULL);
            HMInfo.fNum[2] = strtof(AnalysisData[4],NULL);
            
            //更新巡线环PID
            Algo_PID_Update(&LinePatrolPID,HMInfo.fNum);
            printf("LinePatrolPID %.2f %.2f %.2f\r\n",LinePatrolPID.fKp,LinePatrolPID.fKi,LinePatrolPID.fKd);
        }
    }
}

