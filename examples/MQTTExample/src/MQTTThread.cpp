#include "DataStreams.h"
#include "MQTTThread.h"


void MQTTThread::SetOutDataStream(MQTTBuffStream * ds){
    subbuf = ds;
}

void MQTTThread::SetInDataStream(MQTTBuffStream * ds){
    pubbuf = ds;
}

void MQTTThread::SetLedFlashStream(LEDFlashStream * ds){
    ledstream = ds;
}

bool MQTTThread::reconnect() {
    // Loop until we're reconnected
    if (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "0001";
        String user = "iot@dalen1";
        String pw = "Bing0man!";
        // clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str(), user.c_str(), pw.c_str())) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.publish("~site/iot/0001", "IoT 0001 connected");
            // ... and resubscribe
            client.subscribe("~site/iotnotes");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            return false;
        }
    }
    return true;
}

/*
* This will be called when client.loop() is executed if there is any message
*/
MQTTBuffStream * current_subbuf;
LEDFlashStream * _ledstream;
void callback(char* topic, uint8_t * payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    char c;
    for (int i = 0; i < length; i++) {
        char c = (char)payload[i];
        Serial.print(c);
        current_subbuf->Write(c);
    }
    current_subbuf->Write('\0');
    for (int i = 0; i < 10; i++) {
        _ledstream->Write(95);
    }
    Serial.println();
}

bool MQTTThread::Run()
{
    PT_BEGIN();
    Serial.println("MQTT - starting now");
    client.setClient(espClient);
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    reconnect();
    while (1)
    {
        PT_YIELD();
        _ledstream = ledstream;
        current_subbuf = subbuf; // So that callback uses right stream
        if(client.loop()){ // subscribe poll - returns false if not connected
            if(pubbuf->Count()){
                int i = 0; // If PT_.. macros inside block -> use private variable
                // pubbuf is a circular buffer with 1..n nullterminated strings
                // Take just one null terminated string per publish
                char c = pubbuf->Read();
                while (c && i < (MQTT_MSG_BUF_LEN - 1) && pubbuf->Count()) {
                    msg[i] = c;
                    i++;
                    c = pubbuf->Read();
                }
                msg[i] = '\0';
                // Serial.print("Publishing ");
                // Serial.println(msg);
                client.publish("~site/iot/0001", msg );
            }
        } else {
            if(!reconnect()) {
                timer.Start(5000);
                PT_WAIT_UNTIL(timer.Expired());
            }
        }
    }

    PT_END();
}
