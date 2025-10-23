#include <Arduino.h>
#include "tpl_os.h"

#define LED_PIN 13 
#define pinSwitch 12

const int pinA0 = A0;           // Pin analogic input
const float Vref = 5.0;         // Reference Tension (5V)
const int resolution = 1023;    // ADC resolution 10 bit

static unsigned int press_time_ms = 0;
static bool pressed_flag = false;

DeclareAlarm(a1000msec);
DeclareAlarm(a500msec);
DeclareAlarm(a250msec);
DeclareAlarm(a100msec);

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

void timer_pressure(void)
{
    int A0_valueADC = analogRead(pinA0);                     // analogic read A0
    float A0_voltage = (A0_valueADC * Vref) / resolution;    // Volt
    
    if (digitalRead(pinSwitch) == HIGH) { // Button pressed
        if (!pressed_flag) {
            pressed_flag = true;
            press_time_ms = 0;
        } else {
            press_time_ms += 100; // Assuming this function is called every 100 ms
            if (press_time_ms >= 2000) { // 2 seconds threshold
                ActivateTask(stop);
            }
        }
    } else {
        pressed_flag = false;
        press_time_ms = 0;
    }

    int message = A0_valueADC & 0x3FF;  // bits 0–9
    if (pressed_flag)
        message |= (1 << 12);            // bit 12

    send_to_task_M(message);             // invio messaggio a M
}


TASK(TaskC)
{
    timer_pressure();
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
        TerminateTask(Blink_fast);
        ActivateTask(Blink_slow);
        break;
    elif message = 2:
        TerminateTask(Blink_slow);
        ActivateTask(Blink_fast);
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