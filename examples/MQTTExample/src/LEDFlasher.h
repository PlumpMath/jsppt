#ifndef __LEDFLASHER_H
#define __LEDFLASHER_H

#include "DataStreams.h"

#define BLINK_PIN 5

class LEDFlasher : public StateProtothread
{
public:
    /* The indata stream is an stream of integers with values of 0-100 that
    * sets the ontime in percent for the LED during one second.
    * The stream contains 60 items so it can be set for maximum 1 minute at
    * a time.
    */
    void SetInDataStream(LEDFlashStream * ds);
    virtual bool Run();

private:
    void SetLED(bool on);
    ExpiryTimer timer;
    LEDFlashStream * ontimestream;
    int  ontime;
};

#endif
