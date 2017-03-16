#include "LEDFlasher.h"

void LEDFlasher::SetInDataStream(LEDFlashStream * ds){
    ontimestream = ds;
}

bool LEDFlasher::Run()
{
    PT_BEGIN();
    pinMode(BLINK_PIN, OUTPUT);

    Serial.println("Blink - starting now");

    while (1)
    {
        PT_WAIT_UNTIL(ontimestream->Count());
        ontime = ontimestream->Read();
        ontime = (ontime > 0)?ontime:0;
        ontime = (ontime < 100)?ontime:100;

        #ifdef DEBUG
        Serial.print("Blink ");
        Serial.print(ontime);
        Serial.println("");
        #endif

        // Light the LED longer for stronger RSSIs
        SetLED(true);
        timer.Start(ontime * 10); // ms timer
        PT_WAIT_UNTIL(timer.Expired());

        SetLED(false);
        timer.Start((100 - ontime) * 10);
        PT_WAIT_UNTIL(timer.Expired());
    }

    PT_END();
}

void LEDFlasher::SetLED(bool on)
{
    digitalWrite(BLINK_PIN, (on?HIGH:LOW));
}
