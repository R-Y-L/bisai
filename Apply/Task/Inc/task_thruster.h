#ifndef __TASK_THRUSTER_H_
#define	__TASK_THRUSTER_H_

#define STOP_PWM_VALUE 1500 //停转高电平时间（ns）
#define DEADBAND        0  //死区范围（ns）

void Task_Thruster_Init(void);
void Task_Thruster_SpeedSet(int index,uint16_t _HTime);
void Task_Thruster_AllSpeedSet(uint16_t _HTime);
void Task_Thruster_Start(int index,uint16_t _HTime);
void Task_Thruster_AllStart(float *adress);
void Task_Thruster_Stop(int index);
void Task_Thruster_AllStop(void);

#endif
