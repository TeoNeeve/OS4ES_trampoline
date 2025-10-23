#include "tpl_os.h"
#include <stdio.h>
#include <sys/time.h>

const char *taskNames[] = {
    "Idle Task",		// ID = -1
    "Task Low",			
    "Task Interference",	
    "Task High",		
    "stop"			
};

int	my_time = 0;

void TickerCb_callback(void)
{
	my_time += 1;
}

void PreTaskHook(void)
{
    TaskType currentTask;

    GetTaskID(&currentTask);

    printf( "\n\r%08d: Switching to %s\n\r", my_time, taskNames[currentTask+1] );
}
 

