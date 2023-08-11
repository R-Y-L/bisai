#include "task_conf.h"
#include "usercode.h"
#include "config.h"

extern PWMInfo_T PWMInfo;

void Task_AutoMode_Process(AutoModeInfo AMInfo)
{
    float Angle = 0.0f;         //存储运动角度 [-180,180]
    float CurrYaw = 0.0f;       //当前艏向角来自JY901S
    float ExpYaw = 0.0f;        //期望艏向角
    float PIDOut = 0.0;         //PID计算后的结果
    float Pro = 50;
    float temp;

    //存储黑线角度
    Angle = AMInfo.BlackAngle;

    //设定当前角度和期望角度
    CurrYaw = JY901S.stcAngle.ConYaw;
    ExpYaw = CurrYaw - Angle;
    //printf("ExpYaw %f\r\n",ExpYaw);

    //巡线PID运算
    PIDOut = Algo_PID_Calculate(&LinePatrolPID,CurrYaw,ExpYaw);
    //printf("PIDOut %f\r\n",PIDOut);

    //超过1500us为前进方向
    if(Angle >= -90 && Angle <= 90) //朝第一、二象限运动
    {
        PWMInfo.PWMout[LeftHThruster] =  -PIDOut*Pro/100 + STOP_PWM_VALUE + 100;
        PWMInfo.PWMout[RightHThruster] =  PIDOut*Pro/100 + STOP_PWM_VALUE + 100;

        if(Angle < 0 && Angle >= -90)
        {
            if(PWMInfo.PWMout[LeftHThruster] > PWMInfo.PWMout[RightHThruster])
            {
                temp = PWMInfo.PWMout[LeftHThruster];
                PWMInfo.PWMout[LeftHThruster] = PWMInfo.PWMout[RightHThruster];
                PWMInfo.PWMout[RightHThruster] = temp;   
            }
        }
        else
        {
            if(PWMInfo.PWMout[LeftHThruster] < PWMInfo.PWMout[RightHThruster])
            {
                temp = PWMInfo.PWMout[LeftHThruster];
                PWMInfo.PWMout[LeftHThruster] = PWMInfo.PWMout[RightHThruster];
                PWMInfo.PWMout[RightHThruster] = temp;   
            }
        }
    }
    
    if(Angle == 0)
    {
        PWMInfo.PWMout[LeftHThruster] = 1550;
        PWMInfo.PWMout[RightHThruster] = 1550;
    }

    //PWM限幅，0.6A
    if(PWMInfo.PWMout[LeftHThruster] < 1350)  PWMInfo.PWMout[LeftHThruster] = 1350;
    if(PWMInfo.PWMout[LeftHThruster] > 1650)  PWMInfo.PWMout[LeftHThruster] = 1650;

    if(PWMInfo.PWMout[RightHThruster] < 1350)  PWMInfo.PWMout[RightHThruster] = 1350;
    if(PWMInfo.PWMout[RightHThruster] > 1650)  PWMInfo.PWMout[RightHThruster] = 1650;

    //根据PID结果改变左右翼水平推进器PWM，假设1为左水平推，2为右水平推，超过1500为前进方向
    Task_Thruster_SpeedSet(LeftHThruster,PWMInfo.PWMout[LeftHThruster]);
    Task_Thruster_SpeedSet(RightHThruster,PWMInfo.PWMout[RightHThruster]);

    //printf("Auto left%d right%d\r\n",PWMInfo.PWMout[LeftHThruster],PWMInfo.PWMout[RightHThruster]);
}
