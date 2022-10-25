#include <Arduino.h>
#include "LRTimelapse.h"
#include "configuration.h"
#include "navigation/LRTMenu.h"
#include "navigation/LRTNavigation.h"
#include "lrt_display/LRTDisplay.h"
#include "Camera.h"

namespace LRTimelapse
{

    void setup()
    {
        pinMode(Onboard_LED, OUTPUT);
        digitalWrite(Onboard_LED, LOW); // Turn Onboard LED OFF. it only consumes battery power ;-)

        pinMode(BACK_LIGHT, OUTPUT);
        digitalWrite(BACK_LIGHT, LOW); // First turn backlight off.

        LRTNavigation::setup();

        // wait a moment...  show CAPTION dimming
        /* H.K.: implemented dimming */
        digitalWrite(BACK_LIGHT, HIGH); // Turn backlight on.
        delay(2000);

        LRTDisplay::setupFinished();

        // printIntervalMenu();

        // pinMode(12, OUTPUT);          // initialize output pin for camera release

        // init Timer 1 for Interrupt Timing
        noInterrupts(); // all Interrupts temporary off
        TCCR1A = 0;
        TCCR1B = 0;

        TCNT1 = 64912;          // Timer 10msec (100Hz)
        TCCR1B |= (1 << CS12);  // 256 Prescale
        TIMSK1 |= (1 << TOIE1); // activate Timer Overflow Interrupt n
        interrupts();
    }

    /**
   The main loop
*/
    void loop()
    {
        LRTNavigation::loop();
        Camera::loop();
        
    }

}