#ifndef __TASK_ANALYSISDATA_H_
#define __TASK_ANALYSISDATA_H_

typedef struct 
{
    char ModeChange[15];    //存放模式切换命令
    float fNum[3];          //存放浮点数数据
    bool keyPressed;
}HandleModeInfo;

typedef struct 
{
    char ModeChange[15];    //存放模式切换命令
    float BlackAngle;       //黑线角度
}AutoModeInfo;

void Task_AnalysisData(uint8_t *DataBuf);

#endif

