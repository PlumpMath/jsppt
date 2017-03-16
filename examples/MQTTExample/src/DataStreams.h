#ifndef __DATASTREAMS__H
#define __DATASTREAMS__H

#include "SimplePt.h"

class LEDFlashStream : public SimpleQueue<int,60> {};
class MQTTBuffStream : public SimpleQueue<char, 1024> {};

#endif
