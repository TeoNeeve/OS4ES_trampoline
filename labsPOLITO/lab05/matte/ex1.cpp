#include <Arduino.h>
#include "tpl_os.h"
#include <avr/wdt.h>

#define IN_PIN 0

volatile int elapsed_time = 0;
volatile int digit_value = 0;
volatile int init_time = 0;
volatile bool previous_state = false;
volatile bool PW_error_flag = false;

void setup(void)
{
    Serial.begin(115200);
    pinMode(IN_PIN, INPUT);
    StartOS(OSDEFAULTAPPMODE);
}

void loop(void)
{
    while(1){
        // Task implementations used by the OIL file
    }
}

TASK(TaskA)
{
    GetResource(SensorRes); 
    int sensor_value = digitalRead(IN_PIN);
    if (sensor_value == 1 && !previous_state) {
        previous_state = true;
        elapsed_time = millis() - init_time;
        init_time = millis();
    } else if (sensor_value == 0 && previous_state) {
        previous_state = false;
        digit_value++;
        if (digit_value == 10) {
            digit_value = 0;
        }
        elapsed_time = millis() - init_time;
        if (elapsed_time >= 60) {
            PW_error_flag = true;
        }
        init_time = millis();
        if (elapsed_time > 210) {
            SetEvent(TaskB, PrintEvent);
        }
    }
    ReleaseResource(SensorRes);
    TerminateTask();
}

TASK(TaskB)
{
    while(1) {
        WaitEvent(PrintEvent);
        GetEvent(TaskA, &mask);
        ClearEvent(mask);
        
        if (elapsed_time >= 100) {
            Serial.print(digit_value);
            GetResource(SensorRes);
            digit_value = 0;
            if (PW_error_flag) {
                Serial.print("*");
                PW_error_flag = false;
            }
            ReleaseResource(SensorRes);
            if (elapsed_time > 210){
                Serial.print(" / ");
            }
        }

    TerminateTask();
}