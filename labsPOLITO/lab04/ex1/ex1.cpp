#include <Arduino.h>
#include "tpl_os.h"
#include <avr/wdt.h>

#define K 5
#define LED_PIN 13

DeclareResource(SensorRes);

volatile int Q[K];
volatile int SensorIndex = 0;
volatile int error = 0;
volatile int alarm = 0;

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
    GetResource(SensorRes); 
    Serial.println(X); // DEBUGGING #########################################

    if (X < 10 || X > 1013) {
        error = 1;
    } else {
        error = 0;
        if (SensorIndex < K) {
            Q[SensorIndex] = X;
            SensorIndex++;
        } else {
            Serial.print("Queue overflow by ");
            Serial.println(SensorIndex - K + 1);
        }
    }

    ReleaseResource(SensorRes);
    TerminateTask();
}

TASK(TaskB)
{
    GetResource(SensorRes); 

    SensorIndex = 0; // reset the index for next Q
    int M = Q[0];
    int N = Q[0];
    Serial.println("Dati ricevuti"); // DEBUGGING ###########################
    
    for (int i = 1; i < K; ++i) {
        if (Q[i] > M) {
            M = Q[i];
        }
        if (Q[i] < N) {
            N = Q[i];
        }
    }
    if (M - N > 500) {
        Serial.println("M - N > 500, alarm 1"); // DEBUGGING ################
        alarm = 1;
    } else {
        alarm = 0;
    }

    ReleaseResource(SensorRes); 
    TerminateTask();
}

TASK(TaskV)
{
    GetResource(SensorRes); 

    if (error == 1) {
        CancelAlarm(AlarmBlink);
        SetRelAlarm(AlarmBlink, 125, 125); // 4 Hz
        Serial.println("VELOCE, errore 1"); // DEBUGGING ####################
    } else if (alarm == 1) {
        CancelAlarm(AlarmBlink);
        SetRelAlarm(AlarmBlink, 500, 500); // 1 Hz
        Serial.println("LENTO, errore 0, alarm 1"); // DEBUGGING ############
    } else {
        CancelAlarm(AlarmBlink);
        digitalWrite(LED_PIN, LOW); // OFF
        Serial.println("SPENTO, errore e alarm 0"); // DEBUGGING ############
    }

    ReleaseResource(SensorRes);
    TerminateTask();
}

TASK(Blink)
{   
    static bool led_state = false;
    led_state = !led_state;
    digitalWrite(LED_PIN, led_state ? HIGH : LOW);
    TerminateTask();
}