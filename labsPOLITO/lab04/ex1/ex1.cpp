#include <Arduino.h>
#include "tpl_os.h"
#include <avr/wdt.h>

#define K 5
#define LED_PIN 13

DeclareAlarm(a500msec);
DeclareAlarm(a125msec);
DeclareAlarm(a100msec);

void setup(void)
{
    Serial.begin(115200);
    pinMode(A0,INPUT);
    pinMode(LED_PIN, OUTPUT);
    StartOS(OSDEFAULTAPPMODE);
}

void loop(void)
{
    while(1){
        // Task implementations used by the OIL file
    }
}

TASK(TaskS)
{
    int A0_valueADC = analogRead(A0);
    string SensorReadings[K]
    static int ReadingIndex = 0;
    if (ReadingIndex < K) {
        SensorReadings[K] =
    } else {

    }
    
    ReadingIndex++;
    TerminateTask();
}

TASK(TaskB)
{
    ReadingIndex = 0;

    TerminateTask();
}

TASK(TaskV)
{
    int error
    int alarm
    if (error == 1) {
        CancelAlarm(AlarmBlink);
        SetRelAlarm(AlarmBlink, 125, 125); // 4 Hz
    } else if (alarm == 1) {
        CancelAlarm(AlarmBlink);
        SetRelAlarm(AlarmBlink, 500, 500); // 1 Hz
    } else {
        CancelAlarm(AlarmBlink);
        digitalWrite(LED_PIN, LOW); // OFF
    }
    TerminateTask();
}



