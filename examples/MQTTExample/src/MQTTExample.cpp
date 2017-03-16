/*
*  This program is taken from the Platformio ESP8266 example for WiFIScan
*  and combined with the LEDFlasher example in Protothreads.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "SimplePt.h"
#include "DataStreams.h"
#include "MQTTThread.h"
#include "LEDFlasher.h"
#include "WiFiScanner.h"

// Update these with values suitable for your network.
const char* ssid = "....";
const char* password = "....";

// ------------------------------------

LEDFlashStream ledontime;

MQTTBuffStream mqttPubBuff;
MQTTBuffStream mqttSubBuff;

LEDFlasher flasher1;
WiFiScanner scanner1;
MQTTThread mqttMessenger;

#define NO_PROCS 3
StateProtothread * pt[NO_PROCS] = {&mqttMessenger, &flasher1, &scanner1};

void setup_wifi() {

    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    randomSeed(micros());

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);
    // WiFi.mode(WIFI_STA);
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    // WiFi.disconnect();
    setup_wifi();
    mqttMessenger.SetInDataStream(&mqttPubBuff);
    mqttMessenger.SetOutDataStream(&mqttSubBuff);
    mqttMessenger.SetLedFlashStream(&ledontime);
    flasher1.SetInDataStream(&ledontime);
    scanner1.setMessengerThread(&mqttPubBuff);
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
            #ifdef DEBUG
            if(state != p->GetState()){
                Serial.printf("State for process %u ", i-1);
                Serial.printf("changed from line %u to %u\n", state, p->GetState());
            }
            #endif
        }
    }
    yield();
}
