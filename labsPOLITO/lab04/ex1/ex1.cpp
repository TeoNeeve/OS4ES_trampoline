#include <Arduino.h>
#include "tpl_os.h"
#include <avr/wdt.h>
#define K 5
#define 


DeclareAlarm(a500msec);
DeclareAlarm(a125msec);
DeclareAlarm(a100msec);

void setup(void)
{
    Serial.begin(115200);
    StartOS(OSDEFAULTAPPMODE);
    pinMode(A0,INPUT);
}

void loop(void)
{
    while(1){
        // Task implementations used by the OIL file
    }
}



