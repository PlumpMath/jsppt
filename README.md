## JSP Protothreads

### What it is

JSP Protothreads is a data streams implementation for JSP
(Jackson Structured Programming) together with protothreads. The protothreads
takes care of what is called "program inversion" in JSP. Basically getting
out of one process and into another.

The implementation relies on the [Platform IO](http://platformio.org/) and
Arduino platforms but it is quite simple so porting should not be a problem.
The platform dependent part is in the ExpiryTimer class.

It is based on a [C++ implementation](https://github.com/benhoyt/protothreads-cpp)
by Ben Hoyt of [protothreads](http://dunkels.com/adam/pt/)
by Adam Dunkels.

It implements two type of data streams:
* BufferedDataStream < type, size >. The BufferedDataStream implements a FIFO queue of a certain type and size.
  It connects a writing process with a reader process.
* TimerDataStream.
  The TimerDataStream implements a timer that will send a time event to the
  stream after a delay. Usually the writing process will be the same as the
  reader process.

A process enables reading by calling the Expect() method for the data
stream(s) and then PT_YIELD(). When the process is resumed it shall call the
Read() method. For a TimerDataStream a call to Write(delay) will also enable the
expect flag so in this case it is not necessary to call Expect() too.

Reading from datastreams with timeout is implemented by starting a timer and
expecting events on both the TimerDataStream and BufferedDataStream. Checking
which data stream is written is done with the IsReady() method.

Scheduling is based on data stream events and each data stream need to be
connected to a Protothread. Check the WiFiScan application in the examples
directory for how to implement scheduling and to connect DataStreams and
Protothreads together.

### Some notes

The protothread macros sets up a switch/case statement in your code so take
care when inserting your own. It is also meant for stackless threads so don't
place stack variables in your code, at least not if they pass a macro boundary.
