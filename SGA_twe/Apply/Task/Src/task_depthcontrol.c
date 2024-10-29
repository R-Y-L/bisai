#include "task_conf.h"
#include "usercode.h"
#include "config.h"

extern PWMInfo_T PWMInfo;

/* 深度控制函数，需要当前深度和期望深度 */
void task_DepthControl_Process(float Curr,float Exp)
{
    float PIDOut = 0.0;         //PID计算后的结果

    PIDOut = -Algo_PID_Calculate(&DepthPID,Curr,Exp);

    // printf("%f",PIDOut);
	if(x_y_z_pitch & 0x08)
        {
            PIDOut = speed_kV * 150 / 100;
            PIDOut = 0.0;
        }
	if(x_y_z_pitch & 0x04)
	    {
            PIDOut = -speed_kV * 150 / 100;
            PIDOut = 0.0;
        }
    //实测大于1500ms为上浮
    if(PIDOut > 0)
        PWMInfo.PWMout[h_wheel1_speed] = PIDOut + STOP_PWM_VALUE_F;
    if(PIDOut < 0)
        PWMInfo.PWMout[h_wheel1_speed] = PIDOut + STOP_PWM_VALUE_B;

    //限幅
    if(PWMInfo.PWMout[h_wheel1_speed] < 1380)  PWMInfo.PWMout[h_wheel1_speed] = 1380;
    if(PWMInfo.PWMout[h_wheel1_speed] > 1640)  PWMInfo.PWMout[h_wheel1_speed] = 1640;

    PWMInfo.PWMout[h_wheel2_speed] = PWMInfo.PWMout[h_wheel1_speed];
    PWMInfo.PWMout[h_wheel3_speed] = PWMInfo.PWMout[h_wheel1_speed];
    PWMInfo.PWMout[h_wheel4_speed] = PWMInfo.PWMout[h_wheel1_speed];
    
    // Task_Balance_Process();
}
