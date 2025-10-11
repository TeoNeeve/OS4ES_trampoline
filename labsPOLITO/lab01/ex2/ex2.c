#include <Arduino.h>
#include "tpl_os.h"

#define LED_PIN 13 

DeclareAlarm(a500msec);
DeclareAlarm(a750msec);

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

TASK(TaskA)
{
    digitalWrite(LED_PIN, HIGH);
    TerminateTask();
}

TASK(TaskB)
{
    digitalWrite(LED_PIN, LOW);
    TerminateTask();
}

TASK(stop)
{
	CancelAlarm(a500msec);
	CancelAlarm(a750msec);
	ShutdownOS(E_OK);
	TerminateTask();
}