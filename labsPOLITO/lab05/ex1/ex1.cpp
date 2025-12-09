#include <Arduino.h>
#include "tpl_os.h"
#include <avr/wdt.h>

#define IN_PIN 0

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
    int digit_value = 0;
    int init_time = 0;
    bool previous_state = false;
    bool PW_error_flag = false;
    
    while(1){
        int sensor_value = digitalRead(IN_PIN);
        if (sensor_value == 1 && !previous_state) {
            previous_state = true;
            elapsed_time = millis() - init_time;
            if (elapsed_time >= 100) {
                Serial.print(digit_value);
                digit_value = 0;
                if (PW_error_flag) {
                    Serial.print("*");
                    PW_error_flag = false;
                }
                if (elapsed_time > 200){
                    Serial.print(" / ");
                }
            }
            init_time = millis();
        } else if (sensor_value == 0 && previous_state) {
            previous_state = false;
            digit_value++;
            elapsed_time = millis() - init_time;
            if (elapsed_time >= 60) {
                PW_error_flag = true;
            }
            init_time = millis();
        }
    }
}