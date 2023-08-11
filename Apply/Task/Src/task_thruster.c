#include "task_conf.h"

#include "config.h"

extern PWMInfo_T PWMInfo;
/**
 * @brief 电机初始化
 * @param null
 * @retval null
*/
void Task_Thruster_Init(void)
{
	Drv_Delay_Ms(6000);		/* 初始停转信号后等待稳定 */
	
	/* 停转，1500ms(7.5%占空比) */
	Drv_PWM_HighLvTimeSet(&PWM[0], STOP_PWM_VALUE);
	Drv_PWM_HighLvTimeSet(&PWM[1], STOP_PWM_VALUE);
	Drv_PWM_HighLvTimeSet(&PWM[2], STOP_PWM_VALUE);
	Drv_PWM_HighLvTimeSet(&PWM[3], STOP_PWM_VALUE);
	Drv_Delay_Ms(2000);
}

/**
 * @brief 电机转速设置子函数
 * @param index 选择几号推进器
 * @param _Htime 高电平时间	_Htime可设置为500-2500,1500停止
							500-1500 逆时针旋转 1500-2500顺时针旋转
 * @retval null
*/
void Task_Thruster_SpeedSet(int index,uint16_t _Htime)
{
	Drv_PWM_HighLvTimeSet(&PWM[index], _Htime);
}

/**
 * @brief 所有电机相同转速设置函数
 * @param _Htime 高电平时间	_Htime可设置为500-2500,1500停止
							500-1500 逆时针旋转 1500-2500顺时针旋转
 * @retval null
*/
void Task_Thruster_AllSpeedSet(uint16_t _HTime)
{
	Drv_PWM_HighLvTimeSet(&PWM[0], _HTime);
	Drv_PWM_HighLvTimeSet(&PWM[1], _HTime);
	Drv_PWM_HighLvTimeSet(&PWM[2], _HTime);
	Drv_PWM_HighLvTimeSet(&PWM[3], _HTime);
}	

/**
 * @brief 电机开始工作
 * @param index 选择几号推进器
 * @param _Htime 高电平时间
 * @retval null
*/
void Task_Thruster_Start(int index,uint16_t _HTime)
{
	/* 电机运行速度设置 */
	Task_Thruster_SpeedSet(index,_HTime);
}

/**
 * @brief 所有电机开始工作
 * @param *adress 存放四个推进器的分量数组的首地址
 * @retval null
*/
void Task_Thruster_AllStart(float *adress)
{
	Task_Thruster_Start(0,*adress+STOP_PWM_VALUE);
	Task_Thruster_Start(1,*(adress+1)+STOP_PWM_VALUE);
	Task_Thruster_Start(2,*(adress+2)+STOP_PWM_VALUE);
	Task_Thruster_Start(3,*(adress+3)+STOP_PWM_VALUE);
}

/**
 * @brief 电机停止工作
 * @param index 选择几号推进器
 * @retval null
*/
void Task_Thruster_Stop(int index)
{
	/* 电机运行速度设置 */
	Drv_PWM_HighLvTimeSet(&PWM[index], STOP_PWM_VALUE);
	PWMInfo.PWMout[index] = STOP_PWM_VALUE;
}

/**
 * @brief 所有电机停止工作
 * @param null
 * @retval null
*/
void Task_Thruster_AllStop(void)
{
	Task_Thruster_Stop(0);
	Task_Thruster_Stop(1);
	Task_Thruster_Stop(2);
	Task_Thruster_Stop(3);
}

