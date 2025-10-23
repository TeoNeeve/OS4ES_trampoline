#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include "tpl_os.h"
#include "tpl_com.h"
#include "tpl_posixvp_irq_gen.h"
#include "tpl_os_hooks.h"

#define	DELAY	2000000

int num=0;	
typedef struct myDataTag {
        int     ts;     /* time stamp */
        int     data;   /* samples */
} myData;

void wait_a_lot( long int t )
{
	struct timeval start, end;
	gettimeofday(&start, NULL);

	do {
		gettimeofday(&end, NULL);
	} while( ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) < t );
}

myData generate_next_data( void )
{
        myData tmp;

        tmp.ts = my_time;
        tmp.data = 1+(random() % 1999);  //da cambiare: parti da 1000 e arrivi a 0 scendendo di uno alla volta

        return( tmp );
}

int main(void)
{
	fflush( stdout );
	StartOS(OSDEFAULTAPPMODE);
	return 0;
}

DeclareAlarm(one_sec_producer);
DeclareTask(Producer);
DeclareTask(Consumer);
DeclareMessage(from_producer);
DeclareMessage(consumer_receive);
DeclareResource(sem);

TASK(Producer)
{
	TaskType TaskId;
	myData tmp;

        GetTaskID( &TaskId );

        printf( "\r\n[%04d][PID%d] ", my_time, TaskId );
        fflush( stdout );


	tmp = generate_next_data();
	if( SendMessage(from_producer, &tmp) != E_OK ) {
		printf( "Producer: error sending data\n\r");
	} else {
		GetResource(sem);
		num++;
		printf ("Producer: inserted %04d at time %04d\t\t\t\t %d elements in the buffer\n\r", tmp.data, tmp.ts, num);
		fflush( stdout );
		ReleaseResource(sem);
	}
	TerminateTask();
}

TASK(Consumer)
{
	myData	tmp;

	TaskType TaskId;

        GetTaskID( &TaskId );

	if( ReceiveMessage(consumer_receive, &tmp) == E_COM_LIMIT ) {
		printf( "Consumer: at least one message is lost\n\r" );
	}

	wait_a_lot(DELAY);

	GetResource(sem);
	num--;
	ReleaseResource(sem);

        printf( "\r\n[%04d][PID%d] ", my_time, TaskId );
        fflush( stdout );
	printf ("Consumer: consumed value %04d sampled at time %04d \t\t %d elements in the buffer\n\r", tmp.data, tmp.ts, num);
	fflush( stdout );

	wait_a_lot(DELAY);

	TerminateTask();
}

TASK(stop)
{
  CancelAlarm(one_sec_producer);
  printf("Shutdown\r\n");
  ShutdownOS(E_OK);
  TerminateTask();
}
