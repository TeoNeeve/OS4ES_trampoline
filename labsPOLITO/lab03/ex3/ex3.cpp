#include <Arduino.h>
#include "tpl_os.h"
#include <avr/wdt.h>

#define	A_WCET_CRITIC		200
#define	B_WCET				698 // 700
#define	C_WCET				299
#define	C_WCET_CRITIC		200

#define	A_PERIOD	1000
#define	B_PERIOD	1500
#define	C_PERIOD	2800

DeclareAlarm(activateA);
DeclareAlarm(activateB);
DeclareAlarm(activateC);

void setup(void)
{
	Serial.begin(115200);
	delay(100);  // Give serial time to initialize
	Serial.println("Start...");
    StartOS(OSDEFAULTAPPMODE);
}

void loop(void)
{
    while(1){
        // Task implementations used by the OIL file
    }
}

TASK(MsgInit)
{
	int FreeCriticalMsg = 0;
    SendMessage(send_CriticalMessage, &FreeCriticalMsg);
    TerminateTask();
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
	int ReceivedMsgA;
	int FreeCriticalMsg = 0;
	int deadline_A = countA * A_PERIOD;
	int start_A = millis();
	Serial.print("sA "); // ferma qua durante A2
	Serial.println(start_A);

	do { 
    	ReceiveMessage(CriticalMessage, &ReceivedMsgA);
	} while (ReceivedMsgA != E_OK);

	Serial.println("cA");
	if (countA == 1) {
		do_things(199);
	}
	else {
		do_things(A_WCET_CRITIC);
	}
	int end_A = millis();
	if (end_A > deadline_A) {
		Serial.print("mA ");
		Serial.print(end_A - deadline_A);
	}
	else {
		Serial.print("fA ");
		Serial.println(end_A);
	}
	Serial.println("rA");

	SendMessage(send_CriticalMessage, &FreeCriticalMsg);
	TerminateTask();
}

TASK(TaskB)
{
	static int countB = 0;
	countB++;
	int deadline_B = countB * B_PERIOD;
	int start_B = millis();
	Serial.print("sB ");
	Serial.println(start_B);
	do_things(B_WCET);
	int end_B = millis();
	if (end_B > deadline_B) {
		Serial.print("mB ");
		Serial.print(end_B - deadline_B);
	}
	else {
		Serial.print("fB ");
		Serial.println(end_B);
	}
	TerminateTask();
}

TASK(TaskC)
{
	static int countC = 0;
	countC++;
	int ReceivedMsgC;
	int FreeCriticalMsg = 0;
	int deadline_C = countC * C_PERIOD;
	int start_C = millis();
	Serial.print("sC ");
	Serial.println(start_C);
	do_things(C_WCET - C_WCET_CRITIC);

	do {
    	ReceiveMessage(CriticalMessage, &ReceivedMsgC);
	} while (ReceivedMsgC != E_OK);

	Serial.println("cC");
	do_things(C_WCET_CRITIC);
	int end_C = millis();
	if (end_C > deadline_C) {
		Serial.print("mC ");
		Serial.print(end_C - deadline_C);
	}
	else {
		Serial.print("fC ");
		Serial.println(end_C);
	}
	Serial.println("rC");

	SendMessage(send_CriticalMessage, &FreeCriticalMsg);
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
