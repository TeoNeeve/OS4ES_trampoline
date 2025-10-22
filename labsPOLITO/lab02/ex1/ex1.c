#include <Arduino.h>
#include "tpl_os.h"

#define LED_PIN 13 

DeclareAlarm(a1000msec);
DeclareAlarm(a250msec);

void setup(void)
{
    init();
    pinMode(LED_PIN, OUTPUT);
    StartOS(OSDEFAULTAPPMODE);
    // return 0;
}

void loop(void)
{
    while(1){
        // Task implementations used by the OIL file
    }
}

TASK(TaskV)
{
    if message = 0:
        digitalWrite(LED_PIN, LOW);
        break;
    elif message = 1:
        digitalWrite(LED_PIN, LOW);
        break;
    elif message = 2:
        digitalWrite(LED_PIN, HIGH);
        break;
    elif message = 3:
        digitalWrite(LED_PIN, HIGH);
        break;
    TerminateTask();
}

TASK(Blink_fast)
{
    digitalWrite(LED_PIN, HIGH);
    TerminateTask();
}

TASK(Blink_slow)
{
    digitalWrite(LED_PIN, LOW);
    TerminateTask();
}   


TASK(TaskM)
{
    digitalWrite(LED_PIN, LOW);
    TerminateTask();
}

TASK(stop)
{
	CancelAlarm(a1000msec);
	CancelAlarm(a250msec);
	ShutdownOS(E_OK);
	TerminateTask();
}