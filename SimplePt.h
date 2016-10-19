
#ifndef __SIMPLEPT_H__
#define __SIMPLEPT_H__

#include <Arduino.h>
#include "Protothread.h"

// -----------------------------------------------

class StateProtothread : public Protothread
{
public:
    unsigned int GetState();
};

unsigned int StateProtothread::GetState(){
    return _ptLine;
}

// -----------------------------------------------

class ExpiryTimer
{
public:
    void Start(unsigned long t);
    bool Expired();
    bool NextInterval();

private:
    unsigned long _expiryTime;
    unsigned long _startTime;
};

void ExpiryTimer::Start(unsigned long t)
{
    _startTime = millis();
    _expiryTime = t;
}

bool ExpiryTimer::Expired()
{
    return (millis() - _startTime) >= _expiryTime;
}

bool ExpiryTimer::NextInterval()
{
    if(Expired()){
        _startTime += _expiryTime;
        return true;
    }else{
        return false;
    }
}

// -----------------------------------------------
template <class T, unsigned int N>
class SimpleQueue
{
public:
    SimpleQueue();
    T Read();
    bool Write(T item);
    unsigned int Size();
    unsigned int Count();
private:
    T data[N];
    unsigned int write_cnt, head, tail;
};

template <class T, unsigned int N>
SimpleQueue<T, N>::SimpleQueue() : head(0),tail(0),write_cnt(0) {}

template <class T, unsigned int N>
bool SimpleQueue<T, N>::Write(T item) {
    if(write_cnt < N){
        data[tail] = item;
        tail = (tail + 1) % N;
        write_cnt++;
        return true;
    }else{
        return false;
    }
}

template <class T, unsigned int N>
T SimpleQueue<T, N>::Read() {
    if(write_cnt > 0){
        int val = head;
        head = (head + 1) % N;
        write_cnt--;
        return data[val];
    }else{
        return 0;
    }
}

template <class T, unsigned int N>
unsigned int SimpleQueue<T,N>::Size() { return N; }

template <class T, unsigned int N>
unsigned int SimpleQueue<T,N>::Count() { return write_cnt; }

#endif // __SIMPLEPT_H__
