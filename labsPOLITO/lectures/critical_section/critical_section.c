#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "tpl_os.h"

#define	SEM 1

extern int my_time;

int main(void)
{
    StartOS(OSDEFAULTAPPMODE);
    return 0;
}

DeclareAlarm(one_msec_taskLow);
DeclareAlarm(one_msec_taskHigh);
DeclareTask(TaskLow);
DeclareTask(TaskHigh);
DeclareResource(Sem);

TASK(TaskLow)
{
  int i;

  printf( "\r\n%08d: Task Low starts\r\n", my_time );

  if( SEM )
    GetResource(Sem);

  for(i = 0;i < 10;i++)
  {
    printf("%d/10,", i+1);
    usleep( 10000 );
  }

  printf( "\n\r" );

  printf( "\r\n%08d: Task Low ends\r\n", my_time );
    
  TerminateTask();

  if( SEM )
    ReleaseResource(Sem);
}

TASK(TaskHigh)
{
  int i;

  printf( "\r\n%08d: Task High starts\r\n", my_time );

  if( SEM )
    GetResource(Sem);

  for(i = 0;i < 10;i++ )
    printf("%d/10,",i+1);

  printf( "\n\r" );

  printf( "\r\n%08d: Task High ends\r\n", my_time );
    
  TerminateTask();

  if( SEM )
  ReleaseResource(Sem);
}

TASK(stop)
{
  CancelAlarm(one_msec_taskLow);
  CancelAlarm(one_msec_taskHigh);
  printf("Shutdown\r\n");
  ShutdownOS(E_OK);
  TerminateTask();
}
