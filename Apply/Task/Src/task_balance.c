#include "task_conf.h"
#include "usercode.h"
#include "config.h"

extern PWMInfo_T PWMInfo;
/* 使得左右机翼保持平衡 */
void Task_Balance_Process(void)
{
    float CurrTiltAngle = JY901S.stcAngle.ConPitch;
    float ExpTiltAngle = 0.0f;
    float PIDout;

    PIDout = Algo_PID_Calculate(&BalancePID,CurrTiltAngle,ExpTiltAngle);
    //printf("%f\r\n",PIDout);

    //左翼下沉时，CurrTiltAngle大于0；右侧相反。垂直推进器大于1500时，上浮
    PWMInfo.PWMout[LeftVThruster] = -PIDout + STOP_PWM_VALUE;
    PWMInfo.PWMout[RightVThruster] = PIDout + STOP_PWM_VALUE;

    //PWM限幅，0.6A
    if(PWMInfo.PWMout[LeftVThruster] < 1350)  PWMInfo.PWMout[LeftVThruster] = 1350;
    if(PWMInfo.PWMout[LeftVThruster] > 1650)  PWMInfo.PWMout[LeftVThruster] = 1650;

    if(PWMInfo.PWMout[RightVThruster] < 1350)  PWMInfo.PWMout[RightVThruster] = 1350;
    if(PWMInfo.PWMout[RightVThruster] > 1650)  PWMInfo.PWMout[RightVThruster] = 1650;

    Task_Thruster_SpeedSet(LeftVThruster,PWMInfo.PWMout[LeftVThruster]);
    Task_Thruster_SpeedSet(RightVThruster,PWMInfo.PWMout[RightVThruster]);

    //printf("%d %d\r\n",PWMInfo.PWMout[LeftVThruster],PWMInfo.PWMout[RightVThruster]);
}

