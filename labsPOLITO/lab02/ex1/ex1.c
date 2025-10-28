#include <Arduino.h>
#include "tpl_os.h"

#define LED_PIN 13 
#define pinSwitch 12
#define Switch_THRESHOLD 1000   // milliseconds
#define ADC_10BIT_MASK 0x3FF    // Mask for 10-bit ADC (bits 0–9)
#define PRESS_FLAG_BIT 12       // Bit position for press flag
#define DEBUG_PIN 10            // Debug output pin ######################################

// extern int my_time; usarlo non funziona 

static unsigned int press_time_ms = 0;
static bool pressed_flag = false;
static bool long_pressed_flag = false;
static bool long_press_sent = false; // to send once per long press
// We use the raw ADC reading (0..1023). Do NOT scale to volts when
// encoding into the message so the 10-bit range occupies bits 0..9.

DeclareAlarm(AlarmBlink);
DeclareAlarm(a500msec);
DeclareAlarm(a100msec);

void setup(void)
{
    init();
    pinMode(LED_PIN, OUTPUT);
    pinMode(pinSwitch, INPUT_PULLUP);
    pinMode(DEBUG_PIN, OUTPUT); // out for debug #########################################
    pinMode(A0, INPUT);
    StartOS(OSDEFAULTAPPMODE);
}

void loop(void)
{
    while(1){
        // Task implementations used by the OIL file
    }
}

int debuggatore(void) // debug output on pin 10 ###################################################
{
    static bool debug_led_state = true; 
    debug_led_state = !debug_led_state;
    digitalWrite(DEBUG_PIN, debug_led_state ? HIGH : LOW);
}

int timer_pressure(void)
{
    int A0_valueADC = analogRead(A0); // raw 10-bit ADC read (0..1023)
    if (digitalRead(pinSwitch) == LOW) { // Button pressed, grounded
        if (!pressed_flag) { // First instance being pressed
            pressed_flag = true;
            press_time_ms = 0;
        } else  {
            press_time_ms += 100; // taskC called every 100 ms
            if (press_time_ms >= Switch_THRESHOLD) {
                long_pressed_flag = true; // reached 1s continuous press
            }
        }
    } else {
        pressed_flag = false; // reset all press states
        long_pressed_flag = false;
        long_press_sent = false;
    }

    // Build message: bits 0..9 = ADC value, bit 12 = long-press indicator
    int message = A0_valueADC & ADC_10BIT_MASK;  // bits 0–9: raw ADC
    // Only set the press flag once per long press (edge), not continuously
    if (long_pressed_flag && !long_press_sent) {
        message |= (1 << PRESS_FLAG_BIT);        // bit 12
        long_press_sent = true; // mark as sent
    }
    return message;
}

int message_scheduler(int received_message) // Extracting on the receiver side:
{
    static int scheduled_message = -1;
    static int reference_value = -1;
    int received_adc_value = received_message & ADC_10BIT_MASK; // Value of A0, bits 0..9

    // If the press flag is set, update the reference and do not send a
    // blink command to TaskV (return -1 -> no SendMessage from TaskM).
    if (received_message & (1 << PRESS_FLAG_BIT)) {
        reference_value = received_adc_value; // Set new reference value
        return 0; // Refecence just set, turn LED OFF
    }

    // If we don't have a reference yet, ask TaskV to turn LED ON (code 3)
    if (reference_value == -1) {
        return 3; // LED ON (no reference yet)
    }

    int difference = abs(received_adc_value - reference_value);
    if (difference < 100) {
        scheduled_message = 0; // LED OFF
    } else if (difference < 200) {
        scheduled_message = 1; // Blink slow
    } else {
        scheduled_message = 2; // Blink fast
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
        CancelAlarm(AlarmBlink);
        ActivateTask(Led_OFF);
    } else if (received_message == 1) { // Blink slow
        CancelAlarm(AlarmBlink);
        SetRelAlarm(AlarmBlink, 500, 500); // 1 Hz
    } else if (received_message == 2) { // Blink fast
        CancelAlarm(AlarmBlink);
        SetRelAlarm(AlarmBlink, 125, 125); // 4 Hz
    } else if (received_message == 3) { // LED ON (no ref)
        CancelAlarm(AlarmBlink);
        ActivateTask(Led_ON);
    }
    TerminateTask();
}

TASK(Led_OFF)
{
    digitalWrite(LED_PIN, LOW);
    TerminateTask();
}

TASK(Blink)
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
	CancelAlarm(AlarmBlink);
    CancelAlarm(a500msec);
	CancelAlarm(a100msec);
	ShutdownOS(E_OK);
	TerminateTask();
}