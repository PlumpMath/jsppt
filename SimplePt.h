
#ifndef __SIMPLEPT_H__
#define __SIMPLEPT_H__

#include <Arduino.h>
#include "Protothread.h"

// -----------------------------------------------

class StateProtothread : public Protothread
{
public:
    unsigned int GetState() { return _ptLine; };
};
// -----------------------------------------------

class ExpiryTimer
{
public:
    void Start(unsigned long t){
        _startTime = millis();
        _expiryTime = t;
    };

    bool Expired(){
        return (millis() - _startTime) >= _expiryTime;
    };

    bool NextInterval(){
        if(Expired()){
            _startTime += _expiryTime;
            return true;
        }else{
            return false;
        }
    };

private:
    unsigned long _expiryTime;
    unsigned long _startTime;
};

// -----------------------------------------------
template <class T, unsigned int N>
class SimpleQueue
{
public:
    SimpleQueue() : head(0),tail(0),write_cnt(0) {};
    T Read() {
        if(write_cnt > 0){
            int val = head;
            head = (head + 1) % N;
            write_cnt--;
            return data[val];
        }else{
            return 0;
        }
    };

    bool Write(T item){
        if(write_cnt < N){
            data[tail] = item;
            tail = (tail + 1) % N;
            write_cnt++;
            return true;
        }else{
            return false;
        }
    };
    unsigned int Size() { return N; };
    unsigned int Count() { return write_cnt; };
private:
    T data[N];
    unsigned int write_cnt, head, tail;
};

#endif // __SIMPLEPT_H__
