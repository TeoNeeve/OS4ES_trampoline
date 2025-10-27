#include <Arduino.h>
#include "tpl_os.h"

#define LED_PIN 13 
#define pinSwitch 12
#define Switch_THRESHOLD 1000   // milliseconds
#define ADC_10BIT_MASK 0x3FF    // Mask for 10-bit ADC (bits 0–9)
#define PRESS_FLAG_BIT 12       // Bit position for press flag

const int pinA0 = A0;           // Pin analogic input
const float Vref = 5.0;         // Reference Tension (5V)
const int resolution = 1023;    // ADC resolution 10 bit
static unsigned int press_time_ms = 0;
static bool pressed_flag = false;
static bool long_pressed_flag = false;

DeclareAlarm(AlarmblinkFast);
DeclareAlarm(a500msec);
DeclareAlarm(AlarmblinkSlow);
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

int timer_pressure(void)
{
    int A0_valueADC = analogRead(pinA0);                  // analogic read A0
    float A0_voltage = (A0_valueADC * Vref) / resolution; // Volt
    if (digitalRead(pinSwitch) == HIGH) { // Button pressed
        if (!pressed_flag) { // First instance being pressed
            pressed_flag = true;
            press_time_ms = 0;
        } else {
            press_time_ms += 100; // function called every 100 ms
            if (press_time_ms >= Switch_THRESHOLD) {
                long_pressed_flag = true; // reached 1s continuous press
            }
        }
    } else {
        pressed_flag = false; // reset all press states
        press_time_ms = 0;
        long_pressed_flag = false;
    }

    // Build message: bits 0..9 = ADC value, bit 12 = long-press indicator
    int message = A0_valueADC & ADC_10BIT_MASK;  // bits 0–9
    if (long_pressed_flag) {
        message |= (1 << PRESS_FLAG_BIT);        // bit 12
    }
    return message;
}

int message_scheduler(int received_message) // Extracting on the receiver side:
{
    static int scheduled_message = -1;
    static int reference_value = 0;
    int received_adc_value = received_message & ADC_10BIT_MASK; // Value of A0, with 0-9 bits
    int difference = abs(received_adc_value - reference_value);
    
    if (received_message & (1 << PRESS_FLAG_BIT)) {
        reference_value = received_adc_value; // Set new reference value
    } else if (reference_value == 0) {
        scheduled_message = 3; // LED ON
        return scheduled_message;
    } else if (difference < 100) {
        scheduled_message = 0; // LED OFF
        return scheduled_message;
    } else if (difference < 200) {
        scheduled_message = 1; // Blink slow
        return scheduled_message;
    } else { // difference >= 200
        scheduled_message = 2; // Blink fast
        return scheduled_message;
    }
    
    return scheduled_message;
}

TASK(TaskC)
{
    int message = timer_pressure();
    StatusType status = SendMessage(MsgCtoM, &message); // Send message to TaskM function implemented by osek
    if (status != E_OK)
        printf("Errore nell’invio del messaggio!\n");
    TerminateTask();
}

TASK(TaskM)
{
    int received_message;
    StatusType status = ReceiveMessage(MsgCtoM, &received_message); // Receive message from TaskC function implemented by osek
    if (status != E_OK)
        printf("Errore nella ricezione del messaggio!\n");
    if (received_message & (1 << PRESS_FLAG_BIT))
        printf("Pulsante premuto per più di 1 secondo!\n");

    int scheduled_message = message_scheduler(received_message);

    if (scheduled_message != -1) {
        status = SendMessage(MsgMtoV, &scheduled_message); // Send message to TaskV function implemented by osek
        if (status != E_OK)
            printf("Errore nell’invio del messaggio!\n");
        }
    TerminateTask();
}

TASK(TaskV)
{
    int received_message;   //essendo periodico non serve il terminate task qui
    StatusType status = ReceiveMessage(MsgMtoV, &received_message); // Receive message from TaskM function implemented by osek
    if (received_message == 0) { // LED OFF
        digitalWrite(LED_PIN, LOW);
        return;
    } else if (received_message == 1) { // Blink slow
        ActivateTask(Blink_slow);
        return;
    } else if (received_message == 2) { // Blink fast
        ActivateTask(Blink_fast);
        return;
    } else if (received_message == 3) { // LED ON
        digitalWrite(LED_PIN, HIGH);
        return;
    }
    TerminateTask();
}

TASK(stop)
{
	CancelAlarm(a1000msec);
	CancelAlarm(a250msec);
	ShutdownOS(E_OK);
	TerminateTask();
}