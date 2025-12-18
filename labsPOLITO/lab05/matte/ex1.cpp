#include <Arduino.h>
#include "tpl_os.h"
#include <avr/wdt.h>

#define IN_PIN 0

volatile int digit_value = 0;
volatile bool PW_error_flag = false;
volatile bool end_of_number_detected = false;
volatile bool print_event = false;

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
    int elapsed_time = 0;
    static int init_time = 0;
    static bool previous_state = false;

    GetResource(SensorRes); 
    int sensor_value = digitalRead(IN_PIN);
    if (sensor_value == 1 && !previous_state) {
        previous_state = true;
        elapsed_time = millis() - init_time;
        if (elapsed_time >= 100) {
            if (elapsed_time > 210){
                end_of_number_detected = true;
            }
            print_event = true;
        }
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
    }
    ReleaseResource(SensorRes);
    TerminateTask();
}

TASK(TaskB)
{
    int local_digit = 0;
    bool local_error = false;
    bool local_end = false;

    GetResource(SensorRes);
    if (!print_event) {
        ReleaseResource(SensorRes);
    } else {
        local_digit = digit_value; // save local copies of shared variables   
        local_error = PW_error_flag;
        local_end = end_of_number_detected;

        digit_value = 0; // reset shared variables
        PW_error_flag = false;
        end_of_number_detected = false;
        print_event = false;
        ReleaseResource(SensorRes);

        // print on serial local variables
        Serial.print(local_digit);
        if (local_error) {
            Serial.print("*");
        }
        if (local_end) {
            Serial.println("/");
        }
    }
    TerminateTask();
}