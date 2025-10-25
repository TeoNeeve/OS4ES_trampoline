#include <Arduino.h>
#include "tpl_os.h"

#define LED_PIN 13 
#define pinSwitch 12

const int pinA0 = A0;           // Pin analogic input
const float Vref = 5.0;         // Reference Tension (5V)
const int resolution = 1023;    // ADC resolution 10 bit

static unsigned int press_time_ms = 0;
static bool pressed_flag = false;

DeclareAlarm(a1000msec);
DeclareAlarm(a500msec);
DeclareAlarm(a250msec);
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
    int A0_valueADC = analogRead(pinA0);                     // analogic read A0
    float A0_voltage = (A0_valueADC * Vref) / resolution;    // Volt
    
    if (digitalRead(pinSwitch) == HIGH) { // Button pressed
        if (!pressed_flag) {
            pressed_flag = true;
            press_time_ms = 0;
        } else {
            press_time_ms += 100; // Assuming this function is called every 100 ms
            if (press_time_ms >= 2000) { // 2 seconds threshold
                ActivateTask(stop);
            }
        }
    } else {
        pressed_flag = false;
        press_time_ms = 0;
    }

    int message = A0_valueADC & 0x3FF;  // bits 0–9
    if (pressed_flag)
        message |= (1 << 12);            // bit 12
    
    return message;
}

int message_scheduler(int message)
{
    static int scheduled_message = 0;
    if (message < 256) {
        scheduled_message = 0; // LED OFF
    } else if (message < 512) {
        scheduled_message = 1; // Blink slow
    } else if (message < 768) {
        scheduled_message = 2; // Blink fast
    } else {
        scheduled_message = 3; // LED ON
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
    if (received_message & (1 << 12))
        printf("Pulsante premuto per più di 1 secondo!\n");
    
    message_scheduler = ();
    
    TerminateTask();
}

TASK(TaskV)
{
    if message = 0:
        digitalWrite(LED_PIN, LOW);
        break;
    elif message = 1:
        TerminateTask(Blink_fast);
        ActivateTask(Blink_slow);
        break;
    elif message = 2:
        TerminateTask(Blink_slow);
        ActivateTask(Blink_fast);
        break;
    elif message = 3:
        digitalWrite(LED_PIN, HIGH);
        break;
    TerminateTask();
}

TASK(Blink_fast)
{
    digitalWrite(LED_PIN, HIGH);
    TerminateTask();
}

TASK(Blink_slow)
{
    digitalWrite(LED_PIN, LOW);
    TerminateTask();
}

TASK(stop)
{
	CancelAlarm(a1000msec);
	CancelAlarm(a250msec);
	ShutdownOS(E_OK);
	TerminateTask();
}