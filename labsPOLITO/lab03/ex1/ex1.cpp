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
    StartOS(OSDEFAULTAPPMODE);
}

void loop(void)
{
    while(1){
        // Task implementations used by the OIL file
    }
}

void do_thingsA( int ms )
{
	unsigned long mul = ms * 504UL;
	unsigned long i;
	for(i=0; i<mul; i++) millis();
}

void do_thingsB( int ms )
{
	unsigned long mul = ms * 504UL;
	unsigned long i;
	for(i=0; i<mul; i++) millis();
}

void do_thingsC( int ms )
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
	Serial.print("A ");
	Serial.println(start_A);
	do_thingsA(A_WCET);
	int end_A = millis();
	if (end_A > deadline_A) {
		Serial.print("MissA ");
		Serial.print(end_A - deadline_A);
	}
	else {
		Serial.print("okA ");
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
	Serial.print("B ");
	Serial.println(start_B);
	do_thingsB(B_WCET);
	int end_B = millis();
	if (end_B > deadline_B) {
		Serial.print("MissB ");
		Serial.print(end_B - deadline_B);
	}
	else {
		Serial.print("okB ");
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
	Serial.print("C ");
	Serial.println(start_C);
	do_thingsC(C_WCET);
	int end_C = millis();
	if (end_C > deadline_C) {
		Serial.print("MissC ");
		Serial.print(end_C - deadline_C);
	}
	else {
		Serial.print("okC ");
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
