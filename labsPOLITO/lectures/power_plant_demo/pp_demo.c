#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include "tpl_os.h"
#include "tpl_os_hooks.h"

DeclareAlarm(alarm_250ms);
DeclareAlarm(alarm_500ms);
DeclareAlarm(display_alarm_250ms);

volatile int abort_execution = 0;

int simulate_WCET( int wcet, char *ch )
{
	int	i;

	for( i = 0; i < wcet; i++ )
	{
		if( abort_execution )
		{
			return(1);
		}

		printf( "%s", ch );
		fflush(stdout);

		sleep(1);
	}
	return( 0 );
}

int main(void)
{
	printf("[0000] System started, waiting firts event...\r\n" );
	fflush( stdout );
	abort_execution = 0;
	StartOS(OSDEFAULTAPPMODE);
	return 0;
}

TASK(Acq_AB)
{
	TaskType TaskId;

	GetTaskID( &TaskId );

	printf( "\r\n[%04d][PID%d] ", my_time, TaskId );
	printf( "Acquiring sensors A and B..." );
	fflush( stdout );

	simulate_WCET(10, "=");

	printf( "\r\n[%04d][PID%d] ", my_time, TaskId );
	printf( "done" );
	fflush( stdout );

	TerminateTask();
}

TASK(Acq_CD)
{
	TaskType TaskId;

	GetTaskID( &TaskId );

	printf( "\r\n[%04d][PID%d] ", my_time, TaskId );
	printf( "Acquiring sensors C and D..." );
	fflush( stdout );

	simulate_WCET(10, "=");

	printf( "\r\n[%04d][PID%d] ", my_time, TaskId );
	printf( "done" );
	fflush( stdout );

	TerminateTask();
}

TASK(display)
{
	TaskType TaskId;

	GetTaskID( &TaskId );

	printf( "\r\n[%04d][PID%d] ", my_time, TaskId );
	printf( "Updating display..." );
	fflush( stdout );

	simulate_WCET(2, "=");

	printf( "\r\n[%04d][PID%d] ", my_time, TaskId );
	printf( "done" );
	fflush( stdout );

	SetEvent(TX_normal, TX_go);

	TerminateTask();
}

TASK(TX_normal)
{
	EventMaskType received;
	TaskType TaskId;
	GetTaskID( &TaskId );

	while(1)
	{
		WaitEvent( TX_go | TX_kill );
		GetEvent( TX_normal, &received );	

		if( received & TX_kill )
		{
			ClearEvent( TX_kill );
			printf( "\r\n[%04d][PID%d] ", my_time, TaskId );
			printf( "cancelling regular send..." );
			printf( "done" );
			fflush( stdout );
		}
		if( received & TX_go )
		{
			ClearEvent( TX_go );

			printf( "\r\n[%04d][PID%d] ", my_time, TaskId );
			printf( "sending data to grid coordinator..." );
			fflush( stdout );

			if( simulate_WCET(100, "=") == 0 )
			{
				printf( "\r\n[%04d][PID%d] ", my_time, TaskId );
				printf( "TX done" );
				fflush( stdout );
			}
			else
			{
				printf( "\r\n[%04d][PID%d] ", my_time, TaskId );
				printf( "TX aborted" );
				fflush( stdout );
			}
		}
	}

	TerminateTask();
}

TASK(TX_emergency)
{
	TaskType TaskId;

	GetTaskID( &TaskId );

	printf( "\r\n[%04d][PID%d] ", my_time, TaskId );
	printf( "SENDING EMERGENCY REQUEST TO GRID COORDINATOR..." );
	fflush( stdout );

	simulate_WCET(50, "*");
	abort_execution = 1;

	printf( "\r\n[%04d][PID%d] ", my_time, TaskId );
	printf( "done" );
	fflush( stdout );

	SetEvent( TX_normal, TX_kill );

	CancelAlarm( alarm_250ms );
	CancelAlarm( alarm_500ms );
	CancelAlarm( display_alarm_250ms );

	TerminateTask();
}

TASK(resume_operations)
{
	TaskType TaskId;

	GetTaskID( &TaskId );

	printf( "\r\n[%04d][PID%d] ", my_time, TaskId );
	printf( "Resuming operations..." );
	printf( "done" );
	fflush( stdout );

	abort_execution = 0;

	SetRelAlarm( alarm_250ms, 10, 250 );
	SetRelAlarm( alarm_500ms, 10, 500 );
	SetRelAlarm( display_alarm_250ms, 10, 250 );

	TerminateTask();
}

TASK(stop)
{
	printf("Shutdown\r\n");
	ShutdownOS(E_OK);
	TerminateTask();
}
