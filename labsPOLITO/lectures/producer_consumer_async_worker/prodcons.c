#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include "tpl_os.h"
#include "tpl_os_hooks.h"

#define	BUF_SIZE	50
#define	STACK_SIZE	4

#define	DELAY	2000000

#define	SEM	1
//#define TASK_HOOK	1

typedef struct myDataTag {
	int	ts;	/* time stamp */
	int	data;	/* samples */
} myData;

myData buffer[BUF_SIZE];	/* shared buffer */
int add=0;			/* place to add next element */
int rem=0;			/* place to remove next element */
int num=0;			/* number of elements in the buffer */

volatile int producerActivations = 0;	/* Number of activations of the producer task */
volatile int consumerActivations = 0;	/* Number of activations of the consumer task */

myData consumerWorkerQueue[STACK_SIZE];
int    consumerWorkerQueuePtr = 0;

void consumerWorkerEnque( myData tmp )
{
	consumerWorkerQueue[consumerWorkerQueuePtr] = tmp;
	consumerWorkerQueuePtr++;
}

myData consumerWorkerDeque( void )
{
	myData tmp;
	int i;

	tmp=consumerWorkerQueue[0];
	for( i = 1; i < consumerWorkerQueuePtr; i++ )
		consumerWorkerQueue[i-1] = consumerWorkerQueue[i];
	consumerWorkerQueuePtr--;

	return( tmp );
}

myData generate_next_data( void )
{
	myData tmp;

	tmp.ts = my_time;
	tmp.data = 1+(random() % 1999);

	return( tmp );
}

void check_integrity( void )
{
	if( num != (producerActivations-consumerActivations) )
	{
		printf( "[%04d] Huston, we have a problem! %d elements in the buffer, %d producer activations, %d consumer activations\n\r", 
			my_time, num, producerActivations, consumerActivations );
		fflush( stdout );
	}
}

void wait_a_lot( long int t )
{
	struct timeval start, end;
	gettimeofday(&start, NULL);

	do {
		gettimeofday(&end, NULL);
	} while( ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) < t );
}

int main(void)
{
	fflush( stdout );
	StartOS(OSDEFAULTAPPMODE);
	return 0;
}

DeclareAlarm(one_msec_producer);
DeclareAlarm(one_msec_consumer);
DeclareTask(Producer);
DeclareTask(Consumer);
#ifdef SEM
	DeclareResource(Sem);
#endif

TASK(Producer)
{
	TaskType TaskId;
	myData tmp;
	static int lastTimeRun = -1;    /* last time the task was run */

	producerActivations++;
	GetTaskID( &TaskId );

	printf( "\r\n[%04d][PID%d] ", my_time, TaskId );
	fflush( stdout );

	/* Insert into buffer */
	if (num > BUF_SIZE)
	{
		printf( "Producer: buffer is overflowing\n\r" );	/* buffer overflow */
		fflush( stdout );
	} 
	else
	{
		if(num != BUF_SIZE)				/* block if buffer is full */
		{
			/* if executing here, buffer not full so add element */
			#ifdef SEM
				GetResource( Sem );
			#endif

			tmp = generate_next_data();
			buffer[add] = tmp;
			add = (add+1) % BUF_SIZE;
			num++;

			printf ("Producer: inserted %04d at time %04d\t\t\t\t %d elements in the buffer\n\r", tmp.data, tmp.ts, num);  
			if( lastTimeRun != -1 )
                        {
                                if( my_time-lastTimeRun > 100 )         /* 100 is the period of the producer task */
                                        printf( "*** TIME OVERFLOW ***\n\r" );
                        }

			fflush( stdout );

			#ifdef SEM
				ReleaseResource( Sem );
			#endif
		}
		else
		{
			printf( "Producer: buffer is full\n\r" );
			fflush( stdout );
		}
		
	}
	TerminateTask();
}

TASK(Consumer)
{
	myData	tmp;

	TaskType TaskId;

	consumerActivations++;
	GetTaskID( &TaskId );

	printf( "\r\n[%04d][PID%d] ", my_time, TaskId );
	fflush( stdout );

	if (num < 0)
	{
		printf( "\tConsumer: underflow\n\r" );
		fflush( stdout );
	}
	else
	{
		if( num != 0 )
		{
			/* if executing here, buffer not empty so remove element */
			#ifdef SEM
				GetResource( Sem );
			#endif

			tmp = buffer[rem];
			printf ("Consumer: consumed value %04d sampled at time %04d \t\t %d elements in the buffer\n\r", tmp.data, tmp.ts, num);
			fflush( stdout );

			rem = (rem+1) % BUF_SIZE;
			num--;

			consumerWorkerEnque( tmp );
			ActivateTask( ConsumerWorker );
			#ifdef SEM
				ReleaseResource( Sem );
			#endif
		}
		else
		{
			printf( "\tConsumer: buffer is empty\n\r" );
			fflush( stdout );
		}
	}
	TerminateTask();
}

TASK(ConsumerWorker)
{
        TaskType TaskId;

	myData tmp;

	tmp = consumerWorkerDeque();
        printf( "\r\n[%04d][PID%d] ConsumerWoker Start on %04d sampled at time %04d\n\r", my_time, TaskId, tmp.data, tmp.ts );
        fflush( stdout );
        wait_a_lot(DELAY);
        wait_a_lot(DELAY);
        printf( "\r\n[%04d][PID%d] ConsumerWoker End\n\r", my_time, TaskId );
        fflush( stdout );

        TerminateTask();
}

TASK(stop)
{
  CancelAlarm(one_msec_producer);
  CancelAlarm(one_msec_consumer);
  printf("Shutdown\r\n");
  ShutdownOS(E_OK);
  TerminateTask();
}

char *decode(int s )
{
	switch(s)
	{
		case RUNNING: 
			return( "RUNNING" );
		case WAITING:
			return( "WAITING" );
		case READY:
			return( "READY" );
		case SUSPENDED:
			return( "SUSPENDED" );
		default:
			return( "INVALID TASK" );
	}
	return( "NONE" );
}

void PreTaskHook(void)
{
    #ifdef TASK_HOOK
	//TaskType TaskId;
	TaskStateType Task0, Task1;

	//GetTaskID( &TaskId );

	GetTaskState( 0, &Task0 );
	GetTaskState( 1, &Task1 );

	printf("\r\n[%04d] Context switch: Producer[1] %s - Consumer[0] %s\r\n", my_time, decode(Task1), decode(Task0) );
	check_integrity();
	fflush( stdout );
    #endif
}
