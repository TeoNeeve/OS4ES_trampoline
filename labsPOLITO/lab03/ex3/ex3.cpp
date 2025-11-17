#include <Arduino.h>
#include "tpl_os.h"
#include <avr/wdt.h>

#define	A_WCET_CRITIC		200
#define	B_WCET				700
#define	C_WCET				300
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
	Serial.println("System starting...");
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
	int BusyCriticalMsg = 1;
	int deadline_A = countA * A_PERIOD;
	int start_A = millis();
	Serial.print("startA ");
	Serial.println(start_A);

	do {
    	ReceiveMessage(CriticalMessage, &ReceivedMsgA);
	} while (ReceivedMsgA != 0);

	SendMessage(send_CriticalMessage, &BusyCriticalMsg);

	Serial.println("critA");
	do_things(A_WCET_CRITIC);
	int end_A = millis();
	if (end_A > deadline_A) {
		Serial.print("missA ");
		Serial.print(end_A - deadline_A);
	}
	else {
		Serial.print("okA ");
		Serial.println(end_A);
	}
	Serial.println("relA");

	SendMessage(send_CriticalMessage, &FreeCriticalMsg);
	TerminateTask();
}

TASK(TaskB)
{
	static int countB = 0;
	countB++;
	int deadline_B = countB * B_PERIOD;
	int start_B = millis();
	Serial.print("startB ");
	Serial.println(start_B);
	do_thingsB(B_WCET);
	int end_B = millis();
	if (end_B > deadline_B) {
		Serial.print("missB ");
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
	int ReceivedMsgC;
	int FreeCriticalMsg = 0;
	int BusyCriticalMsg = 1;
	int deadline_C = countC * C_PERIOD;
	int start_C = millis();
	Serial.print("startC ");
	Serial.println(start_C);
	do_things(C_WCET - C_WCET_CRITIC);

	do {
    	ReceiveMessage(CriticalMessage, &ReceivedMsgC);
	} while (ReceivedMsgC != 0);

	SendMessage(send_CriticalMessage, &BusyCriticalMsg);

	Serial.println("critC");
	do_things(C_WCET_CRITIC);
	int end_C = millis();
	if (end_C > deadline_C) {
		Serial.print("missC ");
		Serial.print(end_C - deadline_C);
	}
	else {
		Serial.print("okC ");
		Serial.println(end_C);
	}
	Serial.println("relC");

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
