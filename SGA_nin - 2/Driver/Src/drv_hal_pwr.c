/****************************************************************************

* Sigma�Ŷ�

* �ļ���: drv_hal_pwr.c

* ���ݼ�����PWR��Դ���������ļ�

* �ļ���ʷ��

* �汾��	����		����		˵��

*  2.5   2023-06-03	  �����	�������ļ�

****************************************************************************/
#include "drv_hal_conf.h"

#ifdef DRV_HAL_PWR_ENABLE

/**
 * @brief �ָ�HSEʱ�Ӻ���
 * @param Null
 * @note ����ֹͣģʽ������ʱ���ָ���HSE����ʱ��
 * @retval Null 
*/
static void S_PWR_RestoreHSEClk(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    uint32_t pFLatency = 0;

    /* ���õ�Դ����ʱ�� */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* �����ڲ�RCC�Ĵ�����ȡ�������� */
    HAL_RCC_GetOscConfig(&RCC_OscInitStruct);

    /* ��ֹͣģʽ���Ѻ���������ϵͳʱ��:
    ����HSE��PLL */
    RCC_OscInitStruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState        = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState    = RCC_PLL_ON;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Drv_HAL_Error(__FILE__, __LINE__);
    }

    /* �����ڲ�RCC�Ĵ�����ȡʱ������ */
    HAL_RCC_GetClockConfig(&RCC_ClkInitStruct, &pFLatency);

    /* ѡ�� PLL ��Ϊϵͳʱ��Դ, ������ HCLK��PCLK1 �� PCLK2ʱ�ӷ�Ƶϵ�� */
    RCC_ClkInitStruct.ClockType     = RCC_CLOCKTYPE_SYSCLK;
    RCC_ClkInitStruct.SYSCLKSource  = RCC_SYSCLKSOURCE_PLLCLK;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, pFLatency) != HAL_OK)
    {
        Drv_HAL_Error(__FILE__, __LINE__);
    }

    /* ����ʱ�ӼĴ�����ֵ����SystemCoreClock���� */
    SystemCoreClockUpdate();
}

/**
 * @brief ����˯��ģʽ����
 * @param Null
 * @note ˯��ʱ:
 *       �ر��ں�ʱ�ӣ��ں�ֹͣ���������������У�������ϱ���Ϊ����ִ���µĴ��롣
 *       ���Ѻ�:
 *       ���������жϻ��ѣ��Ƚ����жϣ��˳��жϷ������󣬽���ִ��WFIָ���ĳ���
 *       �����¼����ѣ�ֱ�ӽ���ִ��WFE��ĳ���
 * @retval Null 
*/
void Drv_PWR_EnterSleepMode(void)
{
    /* ��ͣ�δ�ʱ�ӣ���ֹͨ���δ�ʱ���жϻ��� */
    HAL_SuspendTick();

    /* ����˯��ģʽ�����뽫�ڴ˴���ͣ���ȴ����� */
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFI);

    /* �ӵ͹���ģʽ���Ѻ�������ѱ�־λ */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

    /* �����Ѻ󣬻ָ��δ�ʱ�� */
    HAL_ResumeTick();
}

/**
 * @brief ����ֹͣģʽ����
 * @param Null
 * @note ֹͣʱ:
 *       �ر��ں�ʱ�ӣ��ں�ֹͣ��Ƭ������Ҳֹͣ��
 *       ���Ѻ�:
 *       STM32��ʹ��HSI��Ϊϵͳʱ�ӡ�
 *       ����EXTI�жϻ��ѣ��Ƚ����жϣ��˳��жϷ������󣬽���ִ��WFIָ���ĳ���
 *       �����¼����ѣ�ֱ�ӽ���ִ��WFE��ĳ���
 * @retval Null 
*/
void Drv_PWR_EnterStopMode(void)
{
    /* ��ͣ�δ�ʱ�ӣ���ֹͨ���δ�ʱ���жϻ��� */
    HAL_SuspendTick();

    /* ����ֹͣģʽ�����뽫�ڴ˴���ͣ���ȴ�����  */
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_STOPENTRY_WFI);

    /* �ӵ͹���ģʽ���Ѻ�������ѱ�־λ */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

    /* ����HSE����ʱ�� */
    S_PWR_RestoreHSEClk();

    /* �����Ѻ󣬻ָ��δ�ʱ�� */
    HAL_ResumeTick();
}

/**
 * @brief �������ģʽ����
 * @param Null
 * @note ����ʱ:
 *       ȫ���رա�����λ���š�RTC_AF1���ż�WKUP���ţ�����I/O�ھ������ڸ���̬��
 *       ���Ѻ�:
 *       �൱��оƬ��λ���ڳ������Ϊ��ͷ��ʼִ�д��롣
 * @retval Null 
*/
void Drv_PWR_EnterStandByMode(void)
{
    /* ʹ��WKUP���ŵĻ��ѹ��� ��ʹ��PA0�������ش������� */
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

    /* ��ͣ�δ�ʱ�ӣ���ֹͨ���δ�ʱ���жϻ��� */
    HAL_SuspendTick();

    /* �������ģʽ�����Ѻ�оƬ���Ḵλ */
    HAL_PWR_EnterSTANDBYMode();
}

#endif
