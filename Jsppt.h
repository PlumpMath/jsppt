
#ifndef __JSPPT_H__
#define __JSPPT_H__

#include <Protothread.h>
#include <Arduino.h>

enum TimerResolution {Millis, Micros};
class ExpiryTimer
{
public:
    ExpiryTimer();
    void Start(unsigned long t);
    bool Expired();
    bool NextInterval();
    void SetTimerResolution(TimerResolution t);

private:
    unsigned long _expiryTime;
    unsigned long _startTime;
    TimerResolution timer_resolution;
};

ExpiryTimer::ExpiryTimer(){
    timer_resolution = Millis;
}

void ExpiryTimer::Start(unsigned long t)
{
    _startTime = timer_resolution == Millis?millis():micros();
    _expiryTime = t;
}

bool ExpiryTimer::Expired()
{
    if(timer_resolution == Millis)
        return (millis() - _startTime) >= _expiryTime;
    else
        return (micros() - _startTime) >= _expiryTime;

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

void ExpiryTimer::SetTimerResolution(TimerResolution t)
{
    timer_resolution = t;
}
// -----------------------------------------------
class DataStream
{
public:
    DataStream();
    void Expect();
    void UnExpect();
    virtual bool IsReady();
    void Attach(Protothread * p);
    Protothread * GetAttached();
    unsigned int Read();
    bool Write(bool data);
    unsigned int Count();

protected:
    bool expected;
    unsigned int write_cnt;

private:
    Protothread * _p;

};

DataStream::DataStream(){
    write_cnt = 0;
}

void DataStream::Expect(){
    expected = true;
}

void DataStream::UnExpect(){
    expected = false;
}

bool DataStream::IsReady(){
    return expected && write_cnt;
}

void DataStream::Attach(Protothread * p){
    _p = p;
}

Protothread * DataStream::GetAttached(){
    return _p;
}

/*
* In this simplest form set true when written.
*/
bool DataStream::Write(bool data){
    ++write_cnt;
    return true;
}

unsigned int DataStream::Read(){
    expected = false;
    unsigned int val = write_cnt;
    if(write_cnt > 0){
        --write_cnt;
    }
    return val;
}

unsigned int DataStream::Count() { return write_cnt; }


// -----------------------------------------------
enum TimerType {OneShot, Periodic};

class TimerDataStream : public DataStream
{
public:
    TimerDataStream();
    bool IsReady();
    bool Write(unsigned long delay);
    unsigned int Read();
    void SetTimerType(TimerType t);
    void SetTimerResolution(TimerResolution t);

private:
    ExpiryTimer _timer;
    TimerType timer_type;
};


TimerDataStream::TimerDataStream(){
    timer_type = OneShot;
    write_cnt = 0;
}

bool TimerDataStream::IsReady(){
    if(!expected){
        return false;
    }
    if(!write_cnt && _timer.Expired()){
        write_cnt = 1;
    }
    return (bool) write_cnt;
}

bool TimerDataStream::Write(unsigned long delay){
    _timer.Start(delay);
    write_cnt = 0;
    expected = true;
    return true;
}

unsigned int TimerDataStream::Read(){
    if(timer_type == Periodic){
        _timer.NextInterval();
    }
    return DataStream::Read();
}

void TimerDataStream::SetTimerType(TimerType t){
    timer_type = t;
}

void TimerDataStream::SetTimerResolution(TimerResolution t){
    _timer.SetTimerResolution(t);
}

// -----------------------------------------------
template <class T, unsigned int N>
class BufferedDataStream : public DataStream
{
public:
    BufferedDataStream();
    bool IsReady();
    T Read();
    bool Write(T item);
    unsigned int Size();
private:
    T data[N];
    unsigned int head, tail;
};

template <class T, unsigned int N>
BufferedDataStream<T, N>::BufferedDataStream() {
    head = 0; tail = 0;
    DataStream::DataStream();
}

template <class T, unsigned int N>
bool BufferedDataStream<T,N>::IsReady()
{
    return expected && write_cnt;
}

template <class T, unsigned int N>
bool BufferedDataStream<T, N>::Write(T item) {
    if(write_cnt < N){
        data[tail] = item;
        tail = (tail + 1)%N;
        write_cnt++;
        return true;
    }else{
        return false;
    }
}

template <class T, unsigned int N>
T BufferedDataStream<T, N>::Read() {
    expected = false;
    if(write_cnt > 0){
        int val = head;
        head = (head + 1)%N;
        write_cnt--;
        return data[val];
    }else{
        return 0;
    }
}

template <class T, unsigned int N>
unsigned int BufferedDataStream<T,N>::Size() { return N; }

#endif // __JSPPT_H__
