#ifndef __CONFIG_H_
#define __CONFIG_H_

#include "drv_hal_conf.h"
#include "ocd_conf.h"
#include "algo_conf.h"
#include "dev_conf.h"

/* 用户句柄声明包含区 */
extern tagUART_T Uart1;

extern tagUART_T Uart3;

extern tagJY901_T JY901S;

extern tagPWM_T PWM[];

extern tagMS5837_T MS5837;

extern tagPID_T DepthPID;

extern tagPID_T YawPID;

extern tagPID_T LinePatrolPID;

#endif
