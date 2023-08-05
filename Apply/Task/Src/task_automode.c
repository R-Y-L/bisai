#include "task_conf.h"
#include "usercode.h"
#include "config.h"

void Task_AutoMode_Process(AutoModeInfo AMInfo)
{
    float Angle = 0.0f;         //存储运动角度 [-180,180]
    float CurrYaw = 0.0f;       //当前艏向角来自JY901S
    float ExpYaw = 0.0f;        //期望艏向角
    float PIDOut = 0.0;         //PID计算后的结果

    uint16_t leftPWM;
    uint16_t rightPWM;

    //存储黑线角度
    Angle = AMInfo.BlackAngle;

    //设定当前角度和期望角度
    CurrYaw = JY901S.stcAngle.ConYaw;
    ExpYaw = CurrYaw - Angle;
    //printf("ExpYaw %f\r\n",ExpYaw);

    //巡线PID运算
    PIDOut = Algo_PID_Calculate(&LinePatrolPID,CurrYaw,ExpYaw);
    //printf("PIDOut %f\r\n",PIDOut);

    if(Angle >= -90 && Angle <= 90) //朝第一、二象限运动
    {
        leftPWM =  -PIDOut /2 + STOP_PWM_VALUE + 50;   //正负PIDOut看推进器
        rightPWM =  PIDOut /2 + STOP_PWM_VALUE + 50;   
    }
    else
    {
        if(PIDOut > 0)      //朝第三象限运动
        {
            //printf("3\r\n");
            leftPWM =  -PIDOut * 1/4 /2 + STOP_PWM_VALUE -50;
            rightPWM = -PIDOut * 3/4 /2 + STOP_PWM_VALUE -50;
        }
        else                //朝第四象限运动
        {
            //printf("4\r\n");
            leftPWM =  PIDOut * 3/4 /2 + STOP_PWM_VALUE -50;
            rightPWM = PIDOut * 1/4 /2 + STOP_PWM_VALUE -50;
        }
    }

    //PWM限幅
    if(leftPWM < 1350)  leftPWM = 1350;
    if(leftPWM > 1650)  leftPWM = 1650;

    if(rightPWM < 1350)  rightPWM = 1350;
    if(rightPWM > 1650)  rightPWM = 1650;

    //根据PID结果改变左右翼水平推进器PWM，假设1为左水平推，2为右水平推，超过1500为前进方向
    Task_Thruster_SpeedSet(1,leftPWM);
    Task_Thruster_SpeedSet(2,rightPWM);

    printf("Auto left%d right%d\r\n",leftPWM,rightPWM);
}
