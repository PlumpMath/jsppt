/*
*  This program is taken from the Platformio ESP8266 example for WiFIScan
*  and combined with the LEDFlasher example in Protothreads.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "SimplePt.h"

#define BLINK_PIN 5

class MeasBuffStream : public SimpleQueue<int,60> {};

class LEDFlasher : public StateProtothread
{
public:
    void AddMeasurementsStream(MeasBuffStream * ds);
    virtual bool Run();

private:
    void SetLED(bool on);
    ExpiryTimer timer;
    MeasBuffStream * measurements;
    int  meas;
};


void LEDFlasher::AddMeasurementsStream(MeasBuffStream * ds){
    measurements = ds;
}

bool LEDFlasher::Run()
{
    PT_BEGIN();
    pinMode(BLINK_PIN, OUTPUT);

    Serial.println("Blink - starting now");

    while (1)
    {
        PT_WAIT_UNTIL(measurements->Count());
        meas = (100 + measurements->Read()) * 10 ;
        meas = (meas > 0)?meas:0;
        /*
        Serial.print("Blink ");
        Serial.print(meas);
        Serial.println("");
        */
        // Light the LED longer for stronger RSSIs
        SetLED(true);
        timer.Start(meas);
        PT_WAIT_UNTIL(timer.Expired());

        SetLED(false);
        timer.Start(200);
        PT_WAIT_UNTIL(timer.Expired());
    }

    PT_END();
}

void LEDFlasher::SetLED(bool on)
{
    digitalWrite(BLINK_PIN, (on?HIGH:LOW));
}

// ------------------------------------
class WiFiScanner : public StateProtothread
{
public:
    void SetOutDataStream(MeasBuffStream * ds);
    virtual bool Run();

private:
    ExpiryTimer polling_timer;
    ExpiryTimer util_timer;
    MeasBuffStream * measurements;
    uint8_t _i, _n;
};

void WiFiScanner::SetOutDataStream(MeasBuffStream * ds){
    measurements = ds;
}

bool WiFiScanner::Run()
{
    PT_BEGIN();
    WiFi.mode(WIFI_STA);
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    // WiFi.disconnect();
    polling_timer.Start(15000);
    while(1)
    {
        PT_WAIT_UNTIL(polling_timer.Expired());
        polling_timer.NextInterval();  // Periodic timer

        Serial.print( millis());
        Serial.println(" - scan start");

        // WiFi.scanNetworks will return the number of networks found
        WiFi.scanNetworks(true); // Set asynch flag so we do not block

        PT_WAIT_UNTIL((_n = WiFi.scanComplete() >= 0));

        // scanComplete returns when first WiFi id found - wait some more
        util_timer.Start(200);
        PT_WAIT_UNTIL(util_timer.Expired());
        _n = WiFi.scanComplete();

        Serial.print( millis());
        Serial.println(" - scan done");

        if (_n == 0)
        Serial.println("no networks found");
        else if(_n > 0)
        {
            Serial.printf("%u networks found\n", _n);
            for (_i = 0; _i < _n; ++_i)
            {
                // Print SSID and RSSI for each network found
                Serial.printf("%u: %s (%d)", _i + 1, WiFi.SSID(_i).c_str(), WiFi.RSSI(_i));
                Serial.println((WiFi.encryptionType(_i) == ENC_TYPE_NONE)?" ":"*");
                measurements->Write(WiFi.RSSI(_i));
            }
        }
        Serial.println("");
    }

    PT_END();
}

MeasBuffStream meas;
LEDFlasher flasher1;
WiFiScanner scanner1;
#define NO_PROCS 2
StateProtothread * pt[NO_PROCS] = {&flasher1, &scanner1};

void setup() {
    Serial.begin(115200);
    flasher1.AddMeasurementsStream(&meas);
    scanner1.SetOutDataStream(&meas);
}

// #define DEBUG
void loop() {
    StateProtothread * p;
    unsigned int state;

    int i = 0;
    while(i < NO_PROCS){
        p = pt[i];
        ++i;
        if(p->IsRunning()){
            state = p->GetState();
            p->Run();
            if(state != p->GetState()){
                #ifdef DEBUG
                Serial.printf("State for process %u ", i-1);
                Serial.printf("changed from line %u to %u\n", state, p->GetState());
                #endif
                i = 0;  // prioritized scheduler, comment if round-robin
            }
        }
    }
    yield();
}
