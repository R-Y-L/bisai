#include "task_conf.h"
#include "usercode.h"
#include "config.h"

extern PWMInfo_T PWMInfo;

/* 深度控制函数，需要当前深度和期望深度 */
void task_DepthControl_Process(float Curr,float Exp)
{
    float PIDOut = 0.0;         //PID计算后的结果

    PIDOut = Algo_PID_Calculate(&DepthPID,Curr,Exp);

    printf("%f,%f",PIDOut,Exp);

    //实测大于1500ms为上浮
    PWMInfo.PWMout[h_wheel1_speed] = -PIDOut + STOP_PWM_VALUE;

    //限幅
    if(PWMInfo.PWMout[h_wheel1_speed] < 1300)  PWMInfo.PWMout[h_wheel1_speed] = 1300;
    if(PWMInfo.PWMout[h_wheel1_speed] > 1650)  PWMInfo.PWMout[h_wheel1_speed] = 1650;

    PWMInfo.PWMout[h_wheel2_speed] = PWMInfo.PWMout[h_wheel1_speed];
    PWMInfo.PWMout[h_wheel3_speed] = PWMInfo.PWMout[h_wheel1_speed];
    PWMInfo.PWMout[h_wheel4_speed] = PWMInfo.PWMout[h_wheel1_speed];

    Task_Thruster_SpeedSet(h_wheel1_speed,PWMInfo.PWMout[h_wheel1_speed]);
    Task_Thruster_SpeedSet(h_wheel2_speed,PWMInfo.PWMout[h_wheel2_speed]);
    Task_Thruster_SpeedSet(h_wheel3_speed,PWMInfo.PWMout[h_wheel3_speed]);
    Task_Thruster_SpeedSet(h_wheel4_speed,PWMInfo.PWMout[h_wheel4_speed]);
}
