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
	Serial.begin(9600);
	delay(100);  // Give serial time to initialize
	Serial.println("System starting...");
	fflush(stdout);
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
	for (i = 0; i < mul; i++){
		millis();
		wdt_reset(); // Reset the watchdog timer so the microcontroller doesn't crash
	}
}

TASK(TaskA)
{
	static uint32_t countA = 0;
	countA++;
	uint32_t deadline_A = countA * (uint32_t)A_PERIOD;
	uint32_t start_A = millis();
	Serial.print("Started TaskA at ");
	Serial.println(start_A);
	do_things(A_WCET);
	uint32_t end_A = millis();
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
	static uint32_t countB = 0;
	countB++;
	uint32_t deadline_B = countB * (uint32_t)B_PERIOD;
	uint32_t start_B = millis();
	Serial.print("Started TaskB at ");
	Serial.println(start_B);
	do_things(B_WCET);
	uint32_t end_B = millis();
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
	static uint32_t countC = 0;
	countC++;
	uint32_t deadline_C = countC * (uint32_t)C_PERIOD;
	uint32_t start_C = millis();
	Serial.print("Started TaskC at ");
	Serial.println(start_C);
	do_things(C_WCET);
	uint32_t end_C = millis();
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
