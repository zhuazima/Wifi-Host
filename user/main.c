#include "hal_led.h"
#include "hal_cpu.h"
#include "hal_time.h"
#include "OS_System.h"

int main(void)
{

	hal_CPUInit();
	OS_TaskInit();
	hal_TimeInit();
	
	hal_LedInit();
	OS_CreatTask(OS_TASK1,hal_LedProc,1,OS_RUN);
	
	OS_Start();
	
	
	
}

