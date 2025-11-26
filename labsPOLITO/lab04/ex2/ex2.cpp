#include <Arduino.h>
#include "tpl_os.h"
#include <avr/wdt.h>

void setup(void)
{
    StartOS(OSDEFAULTAPPMODE);
}

void loop(void)
{
    while(1){
        // Task implementations used by the OIL file
    }
}
