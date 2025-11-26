#include <Arduino.h>
#include "tpl_os.h"
#include <avr/wdt.h>

#define K 5
#define LED_PIN 13

DeclareAlarm(a500msec);
DeclareAlarm(a125msec);
DeclareAlarm(a100msec);
DeclareAlarm(AlarmBlink);

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
    int X = analogRead(A0);
    static int Q[K];
    static int SensorIndex = 0;
    static int error = 0;
    if (X < 10 || X > 1013) {
        error = 0;
        if (SensorIndex < K) {
            Q[SensorIndex] = X;
        } else {
            Serial.print("Queue overflow by ");
            Serial.println(SensorIndex - K + 1);
        }
    } else {
        error = 1;
    }
    SensorIndex++;
    TerminateTask();
}

TASK(TaskB)
{
    int Q[K];
    int SensorIndex;
    int N;
    int M;
    static int alarm = 0;
    SensorIndex = 0;
    M = Q[0];
    N = Q[0];
    for (int i = 1; i < K; ++i) {
        if (Q[i] > M) {
            M = Q[i];
        }
        if (Q[i] < N) {
            N = Q[i];
        }
    }
    if (M - N > 500) {
        alarm = 1;
    } else {
        alarm = 0;
    }

    TerminateTask();
}

TASK(TaskV)
{
    int alarm;
    int error;
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

TASK(Blink)
{   
    static bool led_state = false;
    led_state = !led_state;
    digitalWrite(LED_PIN, led_state ? HIGH : LOW);
    TerminateTask();
}