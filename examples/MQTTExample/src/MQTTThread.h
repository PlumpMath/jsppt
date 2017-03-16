#ifndef __MQTTTHREAD_H
#define __MQTTTHREAD_H

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "DataStreams.h"

#define MQTT_MSG_BUF_LEN 80
class MQTTThread : public StateProtothread
{
public:
    void SetOutDataStream(MQTTBuffStream * ds);
    void SetInDataStream(MQTTBuffStream * ds);
    void SetLedFlashStream(LEDFlashStream * ds);
    virtual bool Run();
    // void callback(char* topic, uint8_t * payload, unsigned int length);

private:
    // const char* mqtt_server = "192.168.200.67";
    const char* mqtt_server = "192.168.1.8";
    bool reconnect();
    ExpiryTimer timer;
    MQTTBuffStream * pubbuf, * subbuf;
    LEDFlashStream * ledstream;
    WiFiClient espClient;
    PubSubClient client;
    char msg[MQTT_MSG_BUF_LEN];
};

#endif
