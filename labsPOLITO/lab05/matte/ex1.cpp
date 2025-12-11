#include <Arduino.h>
#include "tpl_os.h"
#include <avr/wdt.h>

#define IN_PIN 0

volatile int elapsed_time = 0;
volatile int digit_value = 0;
volatile int init_time = 0;
volatile bool previous_state = false;
volatile bool PW_error_flag = false;

volatile bool is_silence_processed = false; 
volatile int pulse_width = 0;   // Durata livello ALTO
volatile int silence_time = 0;  // Durata livello BASSO

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
    unsigned long current_time = millis();
    ReleaseResource(SensorRes);
    if (sensor_value == 1 && !previous_state) {
        previous_state = true;
        
        elapsed_time = current_time - init_time;
        init_time = current_time;
        if (silence_time > 100) {
            SetEvent(TaskB, PrintEvent);
        }
        is_silence_processed = false;
    } 
    
    else if (sensor_value == 0 && previous_state) {
        previous_state = false;
        digit_value++;
        if (elapsed_time >= 60) {
            PW_error_flag = true;
        }
    //(Monitoraggio fine numero)
    else if (sensor_value == 0 && !previous_state) {
        int current_silence = current_time - init_time;
        
        // Se il silenzio supera 210ms e non abbiamo ancora segnalato la fine
        if (current_silence > 210 && !is_silence_processed) { // 
            elapsed_time = current_silence; // Passiamo questo dato al Task B
            SetEvent(TaskB, PrintEvent);
            is_silence_processed = true;
        }
    }
    }

    TerminateTask();
}

TASK(TaskB)
{
    int local_digit = 0;
    bool local_error = false;
    bool local_end = false;
    while(1) {
        WaitEvent(PrintEvent);
        GetEvent(TaskA, &mask);
        ClearEvent(mask);
        GetResource(SensorRes);
        local_digit = digit_value;       
        local_error = PW_error_flag;

        local_end = end_of_number_detected;
        digit_value = 0;
        PW_error_flag = false;
        end_of_number_detected = false; // sezione critica resetto le variabili globali e le salvo in locali
        ReleaseResource(SensorRes);

        if (local_digit > 0) {
            if (local_digit == 10) {
                Serial.print("0");
            } else {
                Serial.print(local_digit);
            }

            if (local_error) {
                Serial.print("*");
            }
        }

        if (local_end) {
            Serial.println("/");
        }
    }
    
    TerminateTask();
}