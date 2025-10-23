#include <stdio.h>
#include "tpl_os.h"

int main(void)
{
    StartOS(OSDEFAULTAPPMODE);
    return 0;
}

TASK(my_periodic_task)
{
  static int occurence = 0;
    
  occurence++;
  printf("Activation my_periodic_task #%d\r\n",occurence);

  TerminateTask();
}

TASK(my_other_periodic_task)
{
  static int occurence = 0;
    
  occurence++;
  printf("Activation my_other_periodic_task #%d\r\n",occurence);

//  while(1);
    
  TerminateTask();
}

TASK(stop)
{
  CancelAlarm(one_second);
  CancelAlarm(half_second);
  printf("Shutdown\r\n");
  ShutdownOS(E_OK);
  TerminateTask();
}

