#ifndef __WIFISCANNER_H
#define __WIFISCANNER_H

#include "DataStreams.h"

class WiFiScanner : public StateProtothread
{
public:
    void setMessengerThread(MQTTBuffStream * ds);
    virtual bool Run();

private:
    ExpiryTimer polling_timer;
    ExpiryTimer util_timer;
    MQTTBuffStream * mqttpub;
    uint8_t _i, _n;
};

#endif
