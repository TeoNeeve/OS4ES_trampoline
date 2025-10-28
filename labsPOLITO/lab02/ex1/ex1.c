#include <Arduino.h>
#include "tpl_os.h"

#define LED_PIN 13 
#define pinSwitch 12
#define Switch_THRESHOLD 1000   // milliseconds
#define ADC_10BIT_MASK 0x3FF    // Mask for 10-bit ADC (bits 0–9)
#define PRESS_FLAG_BIT 12       // Bit position for press flag

const int resolution = 1023;    // ADC resolution 10 bit
static unsigned int press_time_ms = 0;
static bool pressed_flag = false;
static bool long_pressed_flag = false;
// ?const int pinA0 = A0;           // Pin analogic input
const int Vref = 5;         // Reference Tension (5V)

DeclareAlarm(AlarmBlinkSlow);
DeclareAlarm(a500msec);
DeclareAlarm(AlarmBlinkFast);
DeclareAlarm(a100msec);

void setup(void)
{
    init();
    pinMode(LED_PIN, OUTPUT);
    pinMode(pinSwitch, INPUT_PULLUP);
    pinMode(A0, INPUT);
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
    int A0_valueADC = analogRead(A0); // analogic read A0
    int A0_voltage = (A0_valueADC * Vref) / resolution;
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
    int message = A0_voltage & ADC_10BIT_MASK;  // bits 0–9
    if (long_pressed_flag) {
        message |= (1 << PRESS_FLAG_BIT);        // bit 12
    }
    return message;
}

int message_scheduler(int received_message) // Extracting on the receiver side:
{
    static int scheduled_message = -1;
    static int reference_value = -1;
    int received_adc_value = received_message & ADC_10BIT_MASK; // Value of A0, with 0-9 bits
    int difference = abs(received_adc_value - reference_value);
    
    if (received_message & (1 << PRESS_FLAG_BIT)) {
        reference_value = received_adc_value; // Set new reference value
    } else if (reference_value == -1) {
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
    SendMessage(MsgCtoM_send, &message); // Send message to TaskM function implemented by osek
    TerminateTask();
}

TASK(TaskM)
{
    int received_message;
    ReceiveMessage(MsgCtoM, &received_message); // Receive message from TaskC function implemented by osek
    int scheduled_message = message_scheduler(received_message);

    if (scheduled_message != -1) {
        SendMessage(MsgMtoV_send, &scheduled_message); // Send message to TaskV function implemented by osek
    }
    TerminateTask();
}

TASK(TaskV)
{
    int received_message;
    ReceiveMessage(MsgMtoV, &received_message); // Receive message from TaskM function implemented by osek
    if (received_message == 0) { // LED OFF
        CancelAlarm(AlarmBlinkSlow);
        CancelAlarm(AlarmBlinkFast);
        ActivateTask(Led_OFF);
        return;
    } else if (received_message == 1) { // Blink slow
        CancelAlarm(AlarmBlinkFast);
        SetRelAlarm(AlarmBlinkSlow, 100, 100); // period 1s
        return;
    } else if (received_message == 2) { // Blink fast
        CancelAlarm(AlarmBlinkSlow);
        SetRelAlarm(AlarmBlinkFast, 25, 25); // period 250 ms
        return;
    } else if (received_message == 3) { // LED ON (no ref)
        CancelAlarm(AlarmBlinkSlow);
        CancelAlarm(AlarmBlinkFast);
        ActivateTask(Led_ON);
        return;
    }
    TerminateTask();
}

TASK(Led_OFF)
{
    digitalWrite(LED_PIN, LOW);
    TerminateTask();
}

TASK(Blink_slow)
{   
    static bool led_state = false;
    led_state = !led_state;
    digitalWrite(LED_PIN, led_state ? HIGH : LOW);
    TerminateTask();
}

TASK(Blink_fast)
{   
    static bool led_state = false;
    led_state = !led_state;
    digitalWrite(LED_PIN, led_state ? HIGH : LOW);
    TerminateTask();
}

TASK(Led_ON)
{
    digitalWrite(LED_PIN, HIGH);
    TerminateTask();
}

TASK(stop)
{
	CancelAlarm(AlarmBlinkSlow);
    CancelAlarm(a500msec);
    CancelAlarm(AlarmBlinkFast);
	CancelAlarm(a100msec);
	ShutdownOS(E_OK);
	TerminateTask();
}