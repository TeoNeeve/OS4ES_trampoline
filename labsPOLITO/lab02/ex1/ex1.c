#include <Arduino.h>
#include "tpl_os.h"

#define LED_PIN 13 
#define pinSwitch 12

const int pinA0 = A0;           // Pin analogic input
const float Vref = 5.0;         // Reference Tension (5V)
const int resolution = 1023;    // ADC resolution 10 bit

DeclareAlarm(a1000msec);
DeclareAlarm(a250msec);

void setup(void)
{
    init();
    pinMode(LED_PIN, OUTPUT);
    pinMode(pinSwitch, INPUT_PULLUP);
    StartOS(OSDEFAULTAPPMODE);
}

void loop(void)
{
    while(1){
        // Task implementations used by the OIL file
    }
}

TASK(TaskC)
{
    int A0_valueADC = analogRead(pinA0);                  // analogica read A0
    float A0_voltage = (A0_valueADC * Vref) / resolution;    // Volt
    
}

TASK(TaskM)
{
    digitalWrite(LED_PIN, LOW);
    TerminateTask();
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



TASK(stop)
{
	CancelAlarm(a1000msec);
	CancelAlarm(a250msec);
	ShutdownOS(E_OK);
	TerminateTask();
}