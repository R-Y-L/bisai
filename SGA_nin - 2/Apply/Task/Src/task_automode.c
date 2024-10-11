#include "task_conf.h"
#include "usercode.h"
#include "config.h"

extern PWMInfo_T PWMInfo;

void Task_AutoMode_Process(AutoModeInfo AMInfo)
{
    float Angle = 0.0f;             //�洢�˶��Ƕ� [-180,180]
    float CurrYaw = 0.0f;           //��ǰ���������JY901S
    float ExpYaw = 0.0f;            //���������
    float AngleLoopPIDOut = 0.0;    //�ǶȻ�PID�����Ľ��

    float Offset = 0.0f;            //ֱ��ƫ���� [-100,100]
    float PositionLoopPIDOut = 0.0; //λ�û�PID�����Ľ��

    float Wheel = 0; //��������
    float Pedal = 0; //�ƽ�����

    Angle = AMInfo.BlackAngle;
    //�趨��ǰ�ǶȺ������Ƕ�
    CurrYaw = JY901S.stcAngle.ConYaw;
    ExpYaw = CurrYaw - Angle;

    //�ǶȻ�PID����
    AngleLoopPIDOut = Algo_PID_Calculate(&AngleLoopPID,CurrYaw,ExpYaw);
    if(AngleLoopPIDOut > 90) AngleLoopPIDOut = 90;
    if(AngleLoopPIDOut < -90) AngleLoopPIDOut = -90;
    printf("AngleLoopPIDOut %f\r\n",AngleLoopPIDOut);

    //�洢λ��ƫ��
    Offset = AMInfo.CenterShift / 50;

    //λ�û�PID����
    PositionLoopPIDOut = Algo_PID_Calculate(&PositionLoopPID,Offset,0);
    if(PositionLoopPIDOut > 100) PositionLoopPIDOut = 100;
    if(PositionLoopPIDOut < -100) PositionLoopPIDOut = -100;
    printf("PositionLoopPIDOut %f\r\n",PositionLoopPIDOut);

    if(Offset < 0)    //��Ҫ��ת��ص�ԭλ
    {
        if(Angle >= 0)
        {
            //AngleԽ��,PedalԽС,wheelԽ��
            //Offset�ľ���ֵԽ��PedalԽС,wheelԽ��
            Wheel = A * AngleLoopPIDOut * (1.0f + B * PositionLoopPIDOut/100.0f);
            Pedal = 1000.0f/(D * PositionLoopPIDOut * (1.0f + C * fabs(AngleLoopPIDOut)/90.f));
        }
        else
        {
            //Angle�ľ���ֵԽ��,PedalԽС,wheelԽ��
            //Offset�ľ���ֵԽ��PedalԽС,wheelԽ��
            Wheel = A * AngleLoopPIDOut * (1.0f + B * PositionLoopPIDOut/100.0f);
            Pedal = 1000.0f/(D * PositionLoopPIDOut* (1.0f + C * AngleLoopPIDOut/90.f));      
        }
    }
    else
    {
        if(Angle >= 0)
        {
            //AngleԽ��,PedalԽС,wheelԽ��
            //OffsetԽ��PedalԽ��,wheelԽС
            Wheel = A * AngleLoopPIDOut * (1.0f - fabs(B * PositionLoopPIDOut)/100.0f);
            Pedal = - D * PositionLoopPIDOut /((1.0f + C * fabs(AngleLoopPIDOut)/90.f));
        }
        else
        {
            //Angle�ľ���ֵԽ��,PedalԽС,wheelԽ��
            //OffsetԽ��PedalԽ��,wheelԽС
            Wheel = A * AngleLoopPIDOut * (1.0f - fabs(B * PositionLoopPIDOut)/100.0f);
            Pedal = - D * PositionLoopPIDOut /((1.0f + C * AngleLoopPIDOut/90.f));       
        }  
    }

    printf("Wheel:%f Pedal:%f\r\n",Wheel,Pedal);

    //wheel < 0,��ת��wheel > 0,��ת
    PWMInfo.PWMout[LeftHThruster] = Pedal - Wheel + STOP_PWM_VALUE + DEADBAND;
    PWMInfo.PWMout[RightHThruster] = Pedal + Wheel + STOP_PWM_VALUE + DEADBAND;

    printf("Left: %d Right:%d\r\n",PWMInfo.PWMout[LeftHThruster],PWMInfo.PWMout[RightHThruster]);
    //PWM�޷���0.6A
    if(PWMInfo.PWMout[LeftHThruster] < 1400)  PWMInfo.PWMout[LeftHThruster] = 1400;
    if(PWMInfo.PWMout[LeftHThruster] > 1600)  PWMInfo.PWMout[LeftHThruster] = 1600;

    if(PWMInfo.PWMout[RightHThruster] < 1400)  PWMInfo.PWMout[RightHThruster] = 1400;
    if(PWMInfo.PWMout[RightHThruster] > 1600)  PWMInfo.PWMout[RightHThruster] = 1600;

    //����PID����ı�������ˮƽ�ƽ���PWM������1Ϊ��ˮƽ�ƣ�2Ϊ��ˮƽ�ƣ�����1500Ϊǰ������
    Task_Thruster_SpeedSet(LeftHThruster,PWMInfo.PWMout[LeftHThruster]);
    Task_Thruster_SpeedSet(RightHThruster,PWMInfo.PWMout[RightHThruster]);

    //printf("Auto left%d right%d\r\n",PWMInfo.PWMout[LeftHThruster],PWMInfo.PWMout[RightHThruster]);
}
