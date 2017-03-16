#include <ESP8266WiFi.h>
#include "WiFiScanner.h"

void WiFiScanner::setMessengerThread(MQTTBuffStream * ds)
{
    mqttpub = ds;
}

bool WiFiScanner::Run()
{
    PT_BEGIN();
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
                // Serial.printf("%u: %s (%d)", _i + 1, WiFi.SSID(_i).c_str(), WiFi.RSSI(_i));
                // Serial.println((WiFi.encryptionType(_i) == ENC_TYPE_NONE)?" ":"*");
                char msg[76];
                snprintf (msg, 75, "%u:%s:%d:%s", _i + 1,
                WiFi.SSID(_i).c_str(), WiFi.RSSI(_i),
                (WiFi.encryptionType(_i) == ENC_TYPE_NONE)?" ":"*");
                Serial.println(msg);
                int i2 = 0;
                while(i2 < 75 && msg[i2] ){
                    mqttpub->Write(msg[i2]);
                    i2++;
                }
                mqttpub->Write('\0');
            }
        }
        Serial.println("");
    }

    PT_END();
}
