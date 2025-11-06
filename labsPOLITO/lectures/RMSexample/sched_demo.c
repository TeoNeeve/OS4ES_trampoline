#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include "tpl_os.h"
#include "tpl_com.h"
#include "tpl_posixvp_irq_gen.h"
#include "tpl_os_hooks.h"

#define	MAX_STAT	1000

#define	T1_WCET		5
#define	T2_WCET		15
#define	T3_WCET		20

#define	T1_PERIOD	40
#define	T2_PERIOD	50
#define	T3_PERIOD	200

/*------------------------------------------------------------
	Data structure to keep track of tasks execution
*/
typedef struct taskStatsTag {
	int	aTime;
	int	sTime;
	int	fTime;
} taskStatType;

taskStatType T1[MAX_STAT];
int	nT1=0;
taskStatType T2[MAX_STAT];
int	nT2=0;
taskStatType T3[MAX_STAT];
int	nT3=0;
/*------------------------------------------------------------*/

void simulateWCET( int delay )
{
	int	startTickerTime;

	startTickerTime = TickerTime;
	do {
		;
	} while( (TickerTime-startTickerTime) < delay );
	
}

int main(void)
{
	fflush( stdout );
	StartOS(OSDEFAULTAPPMODE);
	return 0;
}

TASK(TaskT1)
{
	TaskType TaskId;
	static int iter=0;

	iter++;

        GetTaskID( &TaskId );

        printf( "\r\n[%04d][PID%d] T1 starts, instance %d", TickerTime, TaskId, iter );
        fflush( stdout );

	T1[nT1].aTime = (iter-1)*T1_PERIOD;
	T1[nT1].sTime = TickerTime;
	simulateWCET(T1_WCET);
	T1[nT1].fTime = TickerTime;
	nT1++;

        printf( "\r\n[%04d][PID%d] T1 ends ", TickerTime, TaskId );
        fflush( stdout );
	if( iter*T1_PERIOD < TickerTime )
	{
		printf( "T1 time overflow\n\r" );
		fflush( stdout );
		//ShutdownOS(E_OK);
	}

	TerminateTask();
}

TASK(TaskT2)
{
	TaskType TaskId;
	static int iter=0;

	iter++;

        GetTaskID( &TaskId );

        printf( "\r\n[%04d][PID%d] T2 starts, instance %d", TickerTime, TaskId, iter );
        fflush( stdout );

	T2[nT2].aTime = (iter-1)*T2_PERIOD;
	T2[nT2].sTime = TickerTime;
	simulateWCET(T2_WCET);
	T2[nT2].fTime = TickerTime;
	nT2++;

        printf( "\r\n[%04d][PID%d] T2 ends ", TickerTime, TaskId );
        fflush( stdout );
	if( iter*T2_PERIOD < TickerTime )
	{
		printf( "T2 time overflow\n\r" );
		fflush( stdout );
		//ShutdownOS(E_OK);
	}

	TerminateTask();
}

TASK(TaskT3)
{
	TaskType TaskId;
	static int iter=0;

	iter++;

        GetTaskID( &TaskId );

        printf( "\r\n[%04d][PID%d] T3 starts, instance %d", TickerTime, TaskId, iter );
        fflush( stdout );

	T3[nT3].aTime = (iter-1)*T3_PERIOD;
	T3[nT3].sTime = TickerTime;
	simulateWCET(T3_WCET);
	T3[nT3].fTime = TickerTime;
	nT3++;

        printf( "\r\n[%04d][PID%d] T3 ends ", TickerTime, TaskId );
        fflush( stdout );
	if( iter*T3_PERIOD < TickerTime )
	{
		printf( "T3 time overflow\n\r" );
		fflush( stdout );
		//ShutdownOS(E_OK);
	}

	TerminateTask();
}

TASK(stop)
{
	TaskType TaskId;
	int	i;

        GetTaskID( &TaskId );

        printf( "\r\n[%04d][PID%d] Shutdown\n\r", TickerTime, TaskId );
        fflush( stdout );

	printf( "TASK T1 Statistics\n\r" );
	printf( "Instance\tActivation\tStart\t\tFinish\t\tDeadline\tDipatchLatency\tLateness\tRuntime\t\tWCET\n\r" );
	for( i = 0; i < nT1; i++ )
	{
		printf( "%04d\t\t%04d\t\t%04d\t\t%04d\t\t%04d\t\t%04d\t\t%04d\t\t%04d\t\t%04d\n\r", i, 
			T1[i].aTime, T1[i].sTime, T1[i].fTime, (i+1)*T1_PERIOD,
			T1[i].sTime-T1[i].aTime, T1[i].fTime-(i+1)*T1_PERIOD, T1[i].fTime-T1[i].sTime, T1_WCET ); 
	}

	printf( "TASK T2 Statistics\n\r" );
	printf( "Instance\tActivation\tStart\t\tFinish\t\tDeadline\tDipatchLatency\tLateness\tRuntime\t\tWCET\n\r" );
	for( i = 0; i < nT2; i++ )
	{
		printf( "%04d\t\t%04d\t\t%04d\t\t%04d\t\t%04d\t\t%04d\t\t%04d\t\t%04d\t\t%04d\n\r", i, 
			T2[i].aTime, T2[i].sTime, T2[i].fTime, (i+1)*T2_PERIOD,
			T2[i].sTime-T2[i].aTime, T2[i].fTime-(i+1)*T2_PERIOD, T2[i].fTime-T2[i].sTime, T2_WCET ); 
	}

	printf( "TASK T3 Statistics\n\r" );
	printf( "Instance\tActivation\tStart\t\tFinish\t\tDeadline\tDipatchLatency\tLateness\tRuntime\t\tWCET\n\r" );
	for( i = 0; i < nT3; i++ )
	{
		printf( "%04d\t\t%04d\t\t%04d\t\t%04d\t\t%04d\t\t%04d\t\t%04d\t\t%04d\t\t%04d\n\r", i, 
			T3[i].aTime, T3[i].sTime, T3[i].fTime, (i+1)*T3_PERIOD,
			T3[i].sTime-T3[i].aTime, T3[i].fTime-(i+1)*T3_PERIOD, T3[i].fTime-T3[i].sTime, T3_WCET ); 
	}

	ShutdownOS(E_OK);
	TerminateTask();
}
