/****************************************************************************

* Sigma�Ŷ�

* �ļ���: drv_hal_timer.c

* ���ݼ�������ʱ��Timer�����ļ�

* �ļ���ʷ��

* �汾��	����		����		˵��

* 1.0.0a 	2020-02-22	���		�������ļ�

****************************************************************************/
#include "drv_hal_conf.h"

#ifdef DRV_HAL_TIMER_ENABLE

/**
 * @brief ��ʱ��ʱ��ʹ��
 * @param _tTimer-Timer�ṹ��ָ��
 * @retval Null
*/
static void S_TIM_CLKEnable(tagTIM_T *_tTimer) 
{	
	if(_tTimer->tTimerHandle.Instance == TIM1)
	{
		__HAL_RCC_TIM1_CLK_ENABLE();
	}
	else if(_tTimer->tTimerHandle.Instance == TIM2)
	{
		__HAL_RCC_TIM2_CLK_ENABLE();
	}
	else if(_tTimer->tTimerHandle.Instance == TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();
	}
	else if(_tTimer->tTimerHandle.Instance == TIM4)
	{
		__HAL_RCC_TIM4_CLK_ENABLE();
	}
	else if(_tTimer->tTimerHandle.Instance == TIM5)
	{
		__HAL_RCC_TIM5_CLK_ENABLE();
	}
	else if(_tTimer->tTimerHandle.Instance == TIM6)
	{
		__HAL_RCC_TIM6_CLK_ENABLE();
	}
	else if(_tTimer->tTimerHandle.Instance == TIM7)
	{
		__HAL_RCC_TIM7_CLK_ENABLE();
	}
	else if(_tTimer->tTimerHandle.Instance == TIM8)
	{
		__HAL_RCC_TIM8_CLK_ENABLE();
	}
}

/**
 * @brief ��ʱ���ж�����
 * @param _tTimer-Timer�ṹ��ָ��
 * @retval Null
*/
static void S_TIM_NVICConfig(tagTIM_T *_tTimer)
{
	if(_tTimer->tTimerHandle.Instance == TIM2)
	{
		HAL_NVIC_SetPriority(TIM2_IRQn, _tTimer->ucPriority, _tTimer->ucSubPriority);
		HAL_NVIC_EnableIRQ(TIM2_IRQn);
	}
	else if(_tTimer->tTimerHandle.Instance == TIM3)
	{			
		HAL_NVIC_SetPriority(TIM3_IRQn, _tTimer->ucPriority, _tTimer->ucSubPriority);
		HAL_NVIC_EnableIRQ(TIM3_IRQn);
	}
	else if(_tTimer->tTimerHandle.Instance == TIM4)
	{			
		HAL_NVIC_SetPriority(TIM4_IRQn, _tTimer->ucPriority, _tTimer->ucSubPriority);
		HAL_NVIC_EnableIRQ(TIM4_IRQn);
	}
	else if(_tTimer->tTimerHandle.Instance == TIM5)
	{			
		HAL_NVIC_SetPriority(TIM5_IRQn, _tTimer->ucPriority, _tTimer->ucSubPriority);
		HAL_NVIC_EnableIRQ(TIM5_IRQn);
	}
	else if(_tTimer->tTimerHandle.Instance == TIM6)
	{			
		HAL_NVIC_SetPriority(TIM6_IRQn, _tTimer->ucPriority, _tTimer->ucSubPriority);
		HAL_NVIC_EnableIRQ(TIM6_IRQn);
	}
	else if(_tTimer->tTimerHandle.Instance == TIM7)
	{			
		HAL_NVIC_SetPriority(TIM7_IRQn, _tTimer->ucPriority, _tTimer->ucSubPriority);
		HAL_NVIC_EnableIRQ(TIM7_IRQn);
	}
}

/**
 * @brief ��ʱ������
 * @param _tTimer-Timer�ṹ��ָ��
 * @retval Null
*/
static void S_TIM_PramConfig(tagTIM_T *_tTimer)
{	
	if (HAL_TIM_Base_Init(&_tTimer->tTimerHandle) != HAL_OK)
	{
		Drv_HAL_Error(__FILE__, __LINE__);
	}

	S_TIM_NVICConfig(_tTimer);
}

/**
 * @brief ��ʱ������
 * @param _tTimer-Timer�ṹ��ָ��
 * @retval Null
*/
void Drv_Timer_Reset(tagTIM_T *_tTimer)
{
	HAL_TIM_IRQHandler(&(_tTimer->tTimerHandle));
}

/**
 * @brief ��ʱ��ʹ��
 * @param _tTimer-Timer�ṹ��ָ��
 * @retval Null
*/
void Drv_Timer_Enable(tagTIM_T *_tTimer)
{
	HAL_TIM_Base_Start_IT(&(_tTimer->tTimerHandle));
}

/**
 * @brief ��ʱ��ʧ��
 * @param _tTimer-Timer�ṹ��ָ��
 * @retval Null
*/
void Drv_Timer_Disable(tagTIM_T *_tTimer)
{
	HAL_TIM_Base_Stop_IT(&_tTimer->tTimerHandle);
}

/**
 * @brief ��ʱ����ʼ��
 * @param _tTimer-Timer�ṹ��ָ��
 * @param _num-��ʱ������
 * @retval Null
*/
void Drv_Timer_Init(tagTIM_T *_tTimer)
{
	S_TIM_CLKEnable(_tTimer);		/* ʹ��TIMʱ�� */		
	S_TIM_PramConfig(_tTimer);
}

/**
 * @brief ��ʱ���ж�
 * @param _tTimer-Timer�ṹ��ָ��
 * @retval Null
*/
void Drv_Timer_IRQHandler(tagTIM_T *_tTimer)
{
	HAL_TIM_IRQHandler(&_tTimer->tTimerHandle);
}

#endif
