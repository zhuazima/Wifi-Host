/*
 * @Author: your name
 * @Date: 2021-11-20 19:28:52
 * @LastEditTime: 2021-11-22 19:13:47
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \Wifi Host\user\main.c
 */


#include "hal_led.h"
#include "hal_cpu.h"
#include "hal_time.h"
#include "OS_System.h"
#include "app.h"
#include "hal_rfd.h"



int main(void)
{

	hal_CPUInit();
	OS_TaskInit();
	hal_TimeInit();
	
	hal_LedInit();
	OS_CreatTask(OS_TASK1,hal_LedProc,1,OS_RUN);

	hal_RFDInit();
	OS_CreatTask(OS_TASK2,hal_RFDProc,1,OS_RUN);

	AppInit();
	OS_CreatTask(OS_TASK3,AppProc,1,OS_RUN);


	OS_Start();
	
	
	
}

