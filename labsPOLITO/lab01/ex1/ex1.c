#include <stdio.h>
#include <math.h>
#include "tpl_os.h"

int main(void)
{
	printf("lab01\n");
	printf("Task A: period = 500 ms, priority = 2, starts at t=0 ms\n");
	printf("Task B: period = 750 ms, priority = 1, starts at t=1500 ms\n");
	printf("Starting OS...\n\n");
	StartOS(OSDEFAULTAPPMODE);
	return 0;
}

DeclareAlarm(a500msec);
DeclareAlarm(a750msec);

static int counterA = 0;
static int counterB = 1500;

TASK(TaskA)
{
	counterA += 500;
	printf("Task A executed. CounterA = %d\r\n", counterA);
	TerminateTask();
}

TASK(TaskB)
{
	counterB += 750;
	printf("Task B executed. CounterB = %d\r\n", counterB);
	TerminateTask();
}

TASK(stop)
{
	CancelAlarm(a500msec);
	CancelAlarm(a750msec);
	printf("\nStopping periodic tasks...\r\n");
	printf("Final CounterA = %d\r\n", counterA);
	printf("Final CounterB = %d\r\n", counterB);
	printf("Shutdown\r\n");
	ShutdownOS(E_OK);
	TerminateTask();
}
