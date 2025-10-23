#include "tpl_os.h"
#include <stdio.h>
#include <sys/time.h>

const char *taskNames[] = {
    "Idle Task",		// ID = -1
    "my_periodic_task",		// ID = 0
    "my_other_periodic_task",	// ID = 1
    "stop"			// ID = 2
};

int	my_time = 0;

void MyAlarmCb_callback(void)
{
	my_time += 10;
}

void PreTaskHook(void)
{
    TaskType currentTask;

    GetTaskID(&currentTask);

    printf( "%08d: Switching to %s\n\r", my_time, taskNames[currentTask+1] );
}
 

