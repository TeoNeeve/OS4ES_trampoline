#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include "tpl_os.h"
#include "tpl_os_hooks.h"

#define	BUF_SIZE	50

#define	DELAY	2000000

#define	SEM	1
#define TASK_HOOK	1

int buffer[BUF_SIZE];	/* shared buffer */
int add=0;		/* place to add next element */
int rem=0;		/* place to remove next element */
int num=0;	

volatile int producerActivations = 0;
volatile int consumerActivations = 0;

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
	static int i=0;

	TaskType TaskId;

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

			buffer[add] = i;
			add = (add+1) % BUF_SIZE;
			num++;

			printf ("Producer: inserted %d \t\t %d elements in the buffer\n\r", i, num);  
			fflush( stdout );
			i++;

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
	int	i;

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

			i = buffer[rem];
			rem = (rem+1) % BUF_SIZE;
			num--;

			printf ("\r\n\tConsumer: consumed value %d \t\t %d elements in the buffer\n\r", i, num);
			fflush( stdout );
			i = buffer[rem];
			rem = (rem+1) % BUF_SIZE;
			num--;

			ActivateTask(ConsumerWorker); // activate a worker task to simulate processing time
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
	printf( "\r\n[%04d][PID%d] ConsumerWorker Start\n\r", my_time, TaskId );
	fflush( stdout );
	wait_a_lot(DELAY);	// this gives the producer a chance to run BEFORE consumer operation is completed
	wait_a_lot(DELAY);
	printf( "\r\n[%04d][PID%d] ConsumerWorker End\n\r", my_time, TaskId );
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
