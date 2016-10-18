/*
*  This program is taken from the Platformio ESP8266 example for WiFIScan
*  and combined with the LEDFlasher example in Protothreads.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Protothread.h>
#include <Jsppt.h>

#define BLINK_PIN 5  // LED on ESP210 board

class MeasBuffStream : public BufferedDataStream<int,60> {};


class LEDFlasher : public Protothread
{
public:
    void AddTimer(TimerDataStream * ds);
    void AddMeasurementsStream(MeasBuffStream * ds);
    virtual bool Run();

private:
    void SetLED(bool on);
    TimerDataStream * timer;
    MeasBuffStream * measurements;
    int  meas;
};

void LEDFlasher::AddTimer(TimerDataStream * ds){
    timer = ds;
}

void LEDFlasher::AddMeasurementsStream(MeasBuffStream * ds){
    measurements = ds;
}

bool LEDFlasher::Run()
{
    PT_BEGIN();
    pinMode(BLINK_PIN, OUTPUT);

    Serial.println("Blink - starting now");

    while (true)
    {
        measurements->Expect(); PT_YIELD();
        meas = (100 + measurements->Read()) * 10 ;
        meas = (meas > 0)?meas:0;
        /*
        Serial.print("Blink ");
        Serial.print(meas);
        Serial.println("");
        */

        // Light the LED longer for stronger RSSIs
        SetLED(true);
        timer->Write(meas); PT_YIELD(); timer->Read();

        SetLED(false);
        timer->Write(200); PT_YIELD();  timer->Read();
    }

    PT_END();
}

void LEDFlasher::SetLED(bool on)
{
    digitalWrite(BLINK_PIN, (on?HIGH:LOW));
}

// ------------------------------------
class WiFiScanner : public Protothread
{
public:
    void AddPollingTimer(TimerDataStream * ds);
    void SetOutDataStream(MeasBuffStream * ds);
    virtual bool Run();

private:
    TimerDataStream * polling_timer;
    MeasBuffStream * measurements;
    uint8_t _i, _n;
};

void WiFiScanner::AddPollingTimer(TimerDataStream * ds){
    polling_timer = ds;
}

void WiFiScanner::SetOutDataStream(MeasBuffStream * ds){
    measurements = ds;
}

bool WiFiScanner::Run()
{
    PT_BEGIN();
    WiFi.mode(WIFI_STA);
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    // WiFi.disconnect();

    polling_timer->SetTimerType(Periodic);
    polling_timer->Write(15000);
    while(1)
    {
        polling_timer->Expect();
        PT_YIELD();
        polling_timer->Read();
        Serial.print( millis());
        Serial.println(" - scan start");

        // WiFi.scanNetworks will return the number of networks found
        _n = WiFi.scanNetworks();
        Serial.print( millis());
        Serial.println(" - scan done");
        if (_n == 0)
        Serial.println("no networks found");
        else
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

TimerDataStream ledds;
TimerDataStream WiFids;
MeasBuffStream meas;
#define NO_STREAMS 3
DataStream * dsp[NO_STREAMS] = {&ledds, &WiFids, &meas};

LEDFlasher flasher1;
WiFiScanner scanner1;

void setup() {
    Serial.begin(115200);
    delay(100); // Do not use delays in loop

    flasher1.AddTimer(&ledds); ledds.Attach(&flasher1);
    flasher1.AddMeasurementsStream(&meas);meas.Attach(&flasher1);

    scanner1.AddPollingTimer(&WiFids); WiFids.Attach(&scanner1);
    scanner1.SetOutDataStream(&meas);

    flasher1.Run();
    scanner1.Run();
}

// #define DEBUG
void loop() {
    Protothread * p;
    DataStream * ds;

    int i = 0;
    while( i < NO_STREAMS ){
        ds = dsp[i];
        ++i;
        if(ds->IsReady()){
            p = ds->GetAttached();
            if(p->IsRunning()){
                #ifdef DEBUG
                Serial.print("Event on DataStream: ");
                Serial.print(i);
                Serial.print(" - timestamp: ");
                Serial.print(millis());
                Serial.println(" ms");
                #endif
                p->Run();
                i = 0; // Uncomment if prioritized scheduler - start from top
            }
        }
    }
    yield();
}
