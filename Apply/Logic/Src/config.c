#include "config.h"

/* 工程中所用到所有IO
	Thruster
		  PWM1	PE9
		  PWM2	PE11
		  PWM3	PE13
		  PWM4	PE14
		  PWM5	PB6
		  PWM6	PB7
		  PWM7	PB
		  PWM8	PB9
	JY901S
		PA2	PA3 Uart2

	Uart1			Uart2			Uart3			Uart4			Uart5
		TX PA9 			TX PA2			TX PB10			TX PC10			TX PC12
		RX PA10			RX PA3			RX PB11			RX PC11			RX PD2
*/
/* 深度环PID */
tagPID_T DepthPID = 
{
	.fKp = 1,
	.fKi = 0.1,
	.fKd = 0,
};

/* 艏向PID */
tagPID_T YawPID = 
{
	.fKp = 1,
	.fKi = 0.2,
	.fKd = 0.1,
};

/* 角度环PID */
tagPID_T AngleLoopPID = 
{
	.fKp = 1,
	.fKi = 0,
	.fKd = 0,
};

/* 位置环PID */
tagPID_T PositionLoopPID = 
{
	.fKp = 1,
	.fKi = 0,
	.fKd = 0,
};

/* 平衡PID */
tagPID_T BalancePID = 
{
	.fKp = 3,
	.fKi = 0,
	.fKd = 0.1,
};

/* 串口1初始化句柄 */
tagUART_T Uart1 = 
{	
	//串口DMA接收参数配置
	.tUartDMA.bRxEnable					= true,						/* DMA接收使能 */
	.tRxInfo.usDMARxMAXSize             = 100,              		/* 接收数据长度 长度保持在协议最长字节*2以上，确保缓存池一定能够稳定接收一个完整的数据帧*/
};

/* 串口3初始化句柄 */
tagUART_T Uart3 = 
{
	.tUARTHandle.Instance				= USART3,
	//串口DMA接收参数配置
	.tUartDMA.bRxEnable					= true,						/* DMA接收使能 */
	.tRxInfo.usDMARxMAXSize             = 100,              		/* 接收数据长度 长度保持在协议最长字节*2以上，确保缓存池一定能够稳定接收一个完整的数据帧*/
};

/* JY901S参数设置 */
tagJY901_T JY901S = 
{
	.tConfig.ucBaud 	= JY901_RXBAUD_9600,
	.tConfig.ucRate		= JY901_RX_2HZ,
	.tConfig.usType		= JY901_OUTPUT_ANGLE,

	.tUART.tRxInfo.usDMARxMAXSize             	= 100,                 /* 接收数据长度 长度保持在协议最长字节*2以上，确保缓存池一定能够稳定接收一个完整的数据帧*/

    .tUART.tUartDMA.bRxEnable					= true,					/* DMA接收使能 */
};

/* MS5837示例 */
tagMS5837_T MS5837 = 
{
    /* 采样分辨率设置 */
	.setOSR = MS5837_OSR4096,

	/* SCL线 */
	.tIIC.tIICSoft[0].tGPIOInit.Pin 		= GPIO_PIN_6,				/* GPIO引脚 */
	.tIIC.tIICSoft[0].tGPIOInit.Mode 		= GPIO_MODE_OUTPUT_PP,		/* GPIO模式 */
	.tIIC.tIICSoft[0].tGPIOInit.Pull 		= GPIO_NOPULL,				/* GPIO上下拉设置，是否需要上下拉看硬件 */
	.tIIC.tIICSoft[0].tGPIOInit.Speed 		= GPIO_SPEED_FREQ_HIGH,		/* GPIO速度 */	
	.tIIC.tIICSoft[0].tGPIOPort 			= GPIOC,					/* GPIO分组 */

	/* SDA线 */
	.tIIC.tIICSoft[1].tGPIOInit.Pin 		= GPIO_PIN_7,				/* GPIO引脚 */
	.tIIC.tIICSoft[1].tGPIOInit.Mode		= GPIO_MODE_INPUT,			/* GPIO模式 */
	.tIIC.tIICSoft[1].tGPIOInit.Pull		= GPIO_NOPULL,				/* GPIO上下拉设置，是否需要上下拉看硬件 */
	.tIIC.tIICSoft[1].tGPIOInit.Speed		= GPIO_SPEED_FREQ_HIGH,		/* GPIO速度 */	
	.tIIC.tIICSoft[1].tGPIOPort 			= GPIOC,					/* GPIO分组 */
};

/* PWM参数设置 */
tagPWM_T PWM[] =
{
	//默认使用前4个IO口输出PWM
	[0] =
	{
		.tPWMHandle.Instance	= TIM1,         	/* 定时器1 */
		.fDuty					= 7.5,				/* 初始占空比（%） */
		.ulFreq					= 50,				/* 频率（Hz） */
		.ucChannel				= TIM_CHANNEL_2,	/* 通道 */
		.tGPIO.tGPIOInit.Pin	= GPIO_PIN_11,		/* IO映射 */
		.tGPIO.tGPIOPort		= GPIOE,			/* IO组映射 */
		.tGPIO.ucAFMode			= FULL_REMAP,		/* IO重映射模式 */
	},
	[1] =
	{
		.tPWMHandle.Instance	= TIM1,         	/* 定时器1 */
		.fDuty					= 7.5,				/* 初始占空比（%） */
		.ulFreq					= 50,				/* 频率（Hz） */
		.ucChannel				= TIM_CHANNEL_4,	/* 通道 */
		.tGPIO.tGPIOInit.Pin	= GPIO_PIN_14,		/* IO映射 */
		.tGPIO.tGPIOPort		= GPIOE,			/* IO组映射 */
		.tGPIO.ucAFMode			= FULL_REMAP,		/* IO重映射模式 */
	},
	[2] =
	{
		.tPWMHandle.Instance	= TIM4,         	/* 定时器4 */
		.fDuty					= 7.5,				/* 初始占空比（%） */
		.ulFreq					= 50,				/* 频率（Hz） */
		.ucChannel				= TIM_CHANNEL_1,	/* 通道 */
		.tGPIO.tGPIOInit.Pin	= GPIO_PIN_6,		/* IO映射 */
		.tGPIO.tGPIOPort		= GPIOB,			/* IO组映射 */
		.tGPIO.ucAFMode			= NO_REMAP,			/* IO重映射模式 */
	},	
	[3] =
	{
		.tPWMHandle.Instance	= TIM4,         	/* 定时器4 */
		.fDuty					= 7.5,				/* 初始占空比（%） */
		.ulFreq					= 50,				/* 频率（Hz） */
		.ucChannel				= TIM_CHANNEL_3,	/* 通道 */
		.tGPIO.tGPIOInit.Pin	= GPIO_PIN_8,		/* IO映射 */
		.tGPIO.tGPIOPort		= GPIOB,			/* IO组映射 */
		.tGPIO.ucAFMode			= NO_REMAP,			/* IO重映射模式 */
	},	
	//下面4个IO口为备用口  
	[4] =
	{
		.tPWMHandle.Instance	= TIM1,         	/* 定时器1 */
		.fDuty					= 7.5,				/* 初始占空比（%） */
		.ulFreq					= 50,				/* 频率（Hz） */
		.ucChannel				= TIM_CHANNEL_1,	/* 通道 */
		.tGPIO.tGPIOInit.Pin	= GPIO_PIN_9,		/* IO映射 */
		.tGPIO.tGPIOPort		= GPIOE,			/* IO组映射 */
		.tGPIO.ucAFMode			= FULL_REMAP,		/* IO重映射模式 */
	},
	[5] =
	{
		.tPWMHandle.Instance	= TIM4,         	/* 定时器4 */
		.fDuty					= 7.5,				/* 初始占空比（%） */
		.ulFreq					= 50,				/* 频率（Hz） */
		.ucChannel				= TIM_CHANNEL_2,	/* 通道 */
		.tGPIO.tGPIOInit.Pin	= GPIO_PIN_7,		/* IO映射 */
		.tGPIO.tGPIOPort		= GPIOB,			/* IO组映射 */
		.tGPIO.ucAFMode			= NO_REMAP,			/* IO重映射模式 */
	},	
	[6] =
	{
		.tPWMHandle.Instance	= TIM1,         	/* 定时器1 */
		.fDuty					= 7.5,				/* 初始占空比（%） */
		.ulFreq					= 50,				/* 频率（Hz） */
		.ucChannel				= TIM_CHANNEL_3,	/* 通道 */
		.tGPIO.tGPIOInit.Pin	= GPIO_PIN_13,		/* IO映射 */
		.tGPIO.tGPIOPort		= GPIOE,			/* IO组映射 */
		.tGPIO.ucAFMode			= FULL_REMAP,		/* IO重映射模式 */
	},
	[7] =
	{
		.tPWMHandle.Instance	= TIM4,         	/* 定时器4 */
		.fDuty					= 7.5,				/* 初始占空比（%） */
		.ulFreq					= 50,				/* 频率（Hz） */
		.ucChannel				= TIM_CHANNEL_4,	/* 通道 */
		.tGPIO.tGPIOInit.Pin	= GPIO_PIN_9,		/* IO映射 */
		.tGPIO.tGPIOPort		= GPIOB,			/* IO组映射 */
		.tGPIO.ucAFMode			= NO_REMAP,			/* IO重映射模式 */
	},
};
