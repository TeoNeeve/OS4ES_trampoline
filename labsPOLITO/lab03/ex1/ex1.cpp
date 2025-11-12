#include <Arduino.h>
#include "tpl_os.h"

#define	A_WCET		200
#define	B_WCET		700
#define	C_WCET		300

#define	A_PERIOD	1000
#define	B_PERIOD	1500
#define	C_PERIOD	2800

void setup(void)
{
	Serial.begin(9600);
    init();
    ActivateTask(TaskA);
	ActivateTask(TaskB);
	ActivateTask(TaskC);
    StartOS(OSDEFAULTAPPMODE);
}

void loop(void)
{
    while(1){
        // Task implementations used by the OIL file
    }
}

void do_things( int ms )
{
	unsigned long mul = ms * 504UL;
	unsigned long i;
	for (i = 0; i < mul; i++) millis();
}

TASK(TaskA)
{
	unsigned long start_A = millis();
	Serial.print("Started TaskA at ");
	Serial.println(start_A);
	do_things(A_WCET);
	unsigned long end_A = millis();
	Serial.print("Finished TaskA at ");
	Serial.println(end_A);
	TerminateTask();
}

TASK(TaskB)
{
	unsigned long start_B = millis();
	Serial.print("Started TaskB at ");
	Serial.println(start_B);
	do_things(B_WCET);
	unsigned long end_B = millis();
	Serial.print("Finished TaskB at ");
	Serial.println(end_B);
	TerminateTask();
}

TASK(TaskC)
{
	unsigned long start_C = millis();
	Serial.print("Started TaskC at ");
	Serial.println(start_C);
	do_things(C_WCET);
	unsigned long end_C = millis();
	Serial.print("Finished TaskC at ");
	Serial.println(end_C);
	TerminateTask();
}

TASK(stop)
{
	CancelAlarm(activateA);
	CancelAlarm(activateB);
	CancelAlarm(activateC);
	ShutdownOS(E_OK);
	TerminateTask();
}
