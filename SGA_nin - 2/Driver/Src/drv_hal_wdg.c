/****************************************************************************

* Sigma�Ŷ�

* �ļ���: drv_hal_wdg.c

* ���ݼ�����WDG���Ź������ļ�

* �ļ���ʷ��

* �汾��	����		����		˵��

*  2.5   2023-06-03	  �����	�������ļ�

****************************************************************************/
#include "drv_hal_conf.h"

#ifdef DRV_HAL_WDG_ENABLE

/**
 * @brief �������Ź����ú���
 * @param _tIWDG-�������Ź��ṹ��ָ��
 * @note  ���ʱ����㹫ʽ Tout = Prescaler / 40 * Reload (ms)
 *        ÿ������ֵ���ǰ���2,048�����ֵ��һ�룩�������
 * @retval Null 
*/
static void S_IWDG_Config(tagIWDG_T *_tIWDG)
{
    /* �������ʱ��ѡ����ʵ�Ԥ��Ƶֵ������ֵ */
    if(_tIWDG->usResetTime > 13107)
	{
		_tIWDG->tIWDGHandle.Init.Prescaler = IWDG_PRESCALER_256;
		_tIWDG->tIWDGHandle.Init.Reload = (float)_tIWDG->usResetTime / 6.4;
	}
	else if(_tIWDG->usResetTime > 6553)
	{
		_tIWDG->tIWDGHandle.Init.Prescaler = IWDG_PRESCALER_128;
		_tIWDG->tIWDGHandle.Init.Reload = (float)_tIWDG->usResetTime / 3.2;
	}
	else if(_tIWDG->usResetTime > 3276)
	{
		_tIWDG->tIWDGHandle.Init.Prescaler = IWDG_PRESCALER_64;
		_tIWDG->tIWDGHandle.Init.Reload = (float)_tIWDG->usResetTime / 1.6;
	}
	else if(_tIWDG->usResetTime > 1638)
	{
		_tIWDG->tIWDGHandle.Init.Prescaler = IWDG_PRESCALER_32;
		_tIWDG->tIWDGHandle.Init.Reload = (float)_tIWDG->usResetTime / 0.8;
	}
	else if(_tIWDG->usResetTime > 819)
	{
		_tIWDG->tIWDGHandle.Init.Prescaler = IWDG_PRESCALER_16;
		_tIWDG->tIWDGHandle.Init.Reload = (float)_tIWDG->usResetTime / 0.4;
	}
	else if(_tIWDG->usResetTime > 409)
	{
		_tIWDG->tIWDGHandle.Init.Prescaler = IWDG_PRESCALER_8;
		_tIWDG->tIWDGHandle.Init.Reload = (float)_tIWDG->usResetTime / 0.2;
	}
	else
	{
		_tIWDG->tIWDGHandle.Init.Prescaler = IWDG_PRESCALER_4;
		_tIWDG->tIWDGHandle.Init.Reload = (float)_tIWDG->usResetTime / 0.1;
	}

    _tIWDG->tIWDGHandle.Instance = IWDG;
}

/**
 * @brief �������Ź���ʼ��
 * @param _tIWDG-�������Ź��ṹ��ָ��
 * @retval Null 
*/
void Drv_IWDG_Init(tagIWDG_T *_tIWDG)
{
	S_IWDG_Config(_tIWDG);

    HAL_IWDG_Init(&_tIWDG->tIWDGHandle);
}

/**
 * @brief �������Ź�ι��
 * @param _tIWDG-�������Ź��ṹ��ָ��
 * @retval Null 
*/
void Drv_IWDG_Feed(tagIWDG_T *_tIWDG)
{
	HAL_IWDG_Refresh(&_tIWDG->tIWDGHandle);
}

#endif
