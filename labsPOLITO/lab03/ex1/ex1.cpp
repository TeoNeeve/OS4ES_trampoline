#include <Arduino.h>
#include "tpl_os.h"
#include <avr/wdt.h>

#define	A_WCET		200
#define	B_WCET		700
#define	C_WCET		300

#define	A_PERIOD	1000
#define	B_PERIOD	1500
#define	C_PERIOD	2800

DeclareAlarm(activateA);
DeclareAlarm(activateB);
DeclareAlarm(activateC);


void setup(void)
{
	Serial.begin(115200);
	delay(100);
	Serial.println("System starting...");
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
	for(i=0; i<mul; i++) millis();
}

TASK(TaskA)
{
	static int countA = 0;
	countA++;
	int deadline_A = countA * A_PERIOD;
	int start_A = millis();
	Serial.print("Started TaskA at ");
	Serial.println(start_A);
	do_things(A_WCET);
	int end_A = millis();
	if (end_A > deadline_A) {
		Serial.print("TaskA missed its deadline of ");
		Serial.print(deadline_A);
		Serial.print(" ms (finished at ");
		Serial.print(end_A);
		Serial.println(" ms)");
	}
	else {
		Serial.print("Finished TaskA at ");
		Serial.println(end_A);
	}
	TerminateTask();
}

TASK(TaskB)
{
	static int countB = 0;
	countB++;
	int deadline_B = countB * B_PERIOD;
	int start_B = millis();
	Serial.print("Started TaskB at ");
	Serial.println(start_B);
	do_things(B_WCET);
	int end_B = millis();
	if (end_B > deadline_B) {
		Serial.print("TaskB missed its deadline of ");
		Serial.print(deadline_B);
		Serial.print(" ms (finished at ");
		Serial.print(end_B);
		Serial.println(" ms)");
	}
	else {
		Serial.print("Finished TaskB at ");
		Serial.println(end_B);
	}
	TerminateTask();
}

TASK(TaskC)
{
	static int countC = 0;
	countC++;
	int deadline_C = countC * C_PERIOD;
	int start_C = millis();
	Serial.print("Started TaskC at ");
	Serial.println(start_C);
	do_things(C_WCET);
	int end_C = millis();
	if (end_C > deadline_C) {
		Serial.print("TaskC missed its deadline of ");
		Serial.print(deadline_C);
		Serial.print(" ms (finished at ");
		Serial.print(end_C);
		Serial.println(" ms)");
	}
	else {
		Serial.print("Finished TaskC at ");
		Serial.println(end_C);
	}
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
