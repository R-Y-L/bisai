#include "task_conf.h"
#include "usercode.h"
#include "config.h"

extern PWMInfo_T PWMInfo;

/* 深度控制函数，需要当前深度和期望深度 */
void task_DepthControl_Process(float Curr,float Exp)
{
    float PIDOut = 0.0;         //PID计算后的结果

    PIDOut = Algo_PID_Calculate(&DepthPID,Curr,Exp);

    //printf("%f",PIDOut);

    //实测大于1500ms为上浮
    PWMInfo.PWMout[LeftVThruster] = -PIDOut + STOP_PWM_VALUE;

    //限幅
    if(PWMInfo.PWMout[LeftVThruster] < 1350)  PWMInfo.PWMout[LeftVThruster] = 1350;
    if(PWMInfo.PWMout[LeftVThruster] > 1650)  PWMInfo.PWMout[LeftVThruster] = 1650;

    PWMInfo.PWMout[RightVThruster] = PWMInfo.PWMout[LeftVThruster];

    Task_Thruster_SpeedSet(LeftVThruster,PWMInfo.PWMout[LeftVThruster]);
    Task_Thruster_SpeedSet(RightVThruster,PWMInfo.PWMout[RightVThruster]);

}
