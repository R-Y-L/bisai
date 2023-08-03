#ifndef __THREADPOOL_H_
#define __THREADPOOL_H_

/* 线程入口函数声明 */
void DataFromIPC(void* paramenter);
void JY901SReadThread(void* paramenter);
void MS5837ReadThread(void* paramenter);
void MODE_HANDLE(void* paramenter);
void MODE_AUTO(void* paramenter);
void TestThread(void* paramenter);

#endif
