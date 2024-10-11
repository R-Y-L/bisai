#ifndef __TASKMOTIONCONTROL_H_
#define __TASKMOTIONCONTROL_H_

#define MAX_HPOWER 150
#define MAX_VPOWER 200
#define YAWSPEED 50

void Task_Motion_Process(void);
void HandleMode_data_handle(void);
void PWNOutput_limit(void);
void Thruster_nagative_data_handle(float k);
void Balance_data_handle(void);
void Thruster_Ascent_Power_Plus(void);

#endif
