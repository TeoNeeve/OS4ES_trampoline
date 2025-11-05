#include <Arduino.h>
#include "tpl_os.h"

#define LED_PIN 13 
#define pinSwitch 12
#define Switch_THRESHOLD 1000   // milliseconds
#define PRESS_FLAG_BIT 12       // Bit position for new ref
#define ADC_10BIT_MASK 0b001111111111 // Mask for 10-bit ADC (bits 0–9)

static unsigned int press_time_ms = 0;
static bool pressed_flag = false;
static bool long_pressed_flag = false;

DeclareAlarm(AlarmBlink);
// DeclareAlarm(a500msec);
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

int timer_pressure(void) // Message that taskC sends
{
    int A0_valueADC = analogRead(A0); // Raw 10-bit ADC read (0..1023)
    if (digitalRead(pinSwitch) == LOW) { // Button pressed = grounded
        if (!pressed_flag) { // First instance being pressed
            pressed_flag = true;
            press_time_ms = 0;
        } else  {
            press_time_ms += 100; // taskC called every 100 ms
            if (press_time_ms >= Switch_THRESHOLD) {
                long_pressed_flag = true; // Reached 1s continuous press
            }
        }
    } else {
        pressed_flag = false; // Reset all press states
        long_pressed_flag = false;
    }

    // Build message: bits 0..9 = ADC value, bit 12 = long-press indicator
    int message = A0_valueADC & ADC_10BIT_MASK;  // bits 0–9: raw ADC
    if (long_pressed_flag) {
        message |= (1 << PRESS_FLAG_BIT); // Setting bit 12 = 1
    }
    return message;
}

int message_scheduler(int received_message) // Extracting on taskM side
{
    static int reference_value = -1;
    int scheduled_message = -1;
    int received_adc_value = received_message & ADC_10BIT_MASK; // Value of A0, bits 0..9

    // If the press flag is set, update the reference using raw ADC value
    if (received_message & (1 << PRESS_FLAG_BIT)) {
        reference_value = received_adc_value;
        scheduled_message = 0;
        return scheduled_message;
    }

    // If we don't have a reference yet turn LED ON
    if (reference_value == -1) {
        scheduled_message = 3;
        return scheduled_message;
    }
    
    // Compare raw ADC values (0-1023 range), 1 bit = ~4.9mV
    int difference = abs(received_adc_value - reference_value);
    if (difference < 20) { // 100 mV approximately 20 ADC units
        scheduled_message = 0; // LED OFF
    } else  if (difference < 40) { // 200 mV approximately 40 ADC units
        scheduled_message = 1; // Blink slow
    } else {
        scheduled_message = 2; // Blink fast
    }
    return scheduled_message;
}

TASK(TaskC)
{
    int message_C = timer_pressure();
    SendMessage(MsgCtoM_send, &message_C); // Send message to TaskM
    TerminateTask();
}

TASK(TaskM)
{
    int received_message_C;
    ReceiveMessage(MsgCtoM, &received_message_C); // Receive message from TaskC
    int scheduled_message_V = message_scheduler(received_message_C);

    if (scheduled_message_V != -1) {
        SendMessage(MsgMtoV_send, &scheduled_message_V); // Send message to TaskV
    }
    TerminateTask();
}

TASK(TaskV)
{
    int received_message_V;
    ReceiveMessage(MsgMtoV, &received_message_V); // Receive message from TaskM
    if (received_message_V == 0) {
        CancelAlarm(AlarmBlink);
        digitalWrite(LED_PIN, LOW); // LED OFF
    } else if (received_message_V == 1) { // Blink slow
        CancelAlarm(AlarmBlink);
        SetRelAlarm(AlarmBlink, 500, 500); // 1 Hz
    } else if (received_message_V == 2) { // Blink fast
        CancelAlarm(AlarmBlink);
        SetRelAlarm(AlarmBlink, 125, 125); // 4 Hz
    } else if (received_message_V == 3) {
        CancelAlarm(AlarmBlink);
        digitalWrite(LED_PIN, HIGH);// LED ON (no ref)
    }
    TerminateTask();
}

TASK(Blink)
{   
    static bool led_state = false;
    led_state = !led_state;
    digitalWrite(LED_PIN, led_state ? HIGH : LOW);
    TerminateTask();
}

TASK(stop)
{
	CancelAlarm(AlarmBlink);
    // CancelAlarm(a500msec);
	CancelAlarm(a100msec);
	ShutdownOS(E_OK);
	TerminateTask();
}