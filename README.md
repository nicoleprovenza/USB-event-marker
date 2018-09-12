# USB Event Marker

Use an arduino-compatible board to output 8-channel digital event markers from a computer to a data acquisition system. When the board receives an event, one or more of the 8 pins will output a short pulse simultaneously. The different combinations of 8 pins can encode 255 distinct event numbers in binary, for identifying different categories of events.

This repository contains:

* Firmware for the Teensy LC or Teensy 3.2 boards, which will listen for events over the USB serial connection. 

* An example python script for sending events to the board over a USB serial connection. Any other language or program that supports writing to a serial port can be used instead. For each event, it should write a single-byte number between 1 and 255 to the serial port. That's the number that will be encoded in binary on the output pins.

## Timing

Try not to send two event markers that are closer together than the length of the pulse (10ms by default, controlled by `pulse_milliseconds` in the firmware). If you do, you'll see two full pulses with a very short gap between them (~10us by default, controlled by `gap_microseconds`). So it will look as though the second event happened right after the first pulse ended, even though in reality, it happened earlier than that.

Make sure that `pulse_milliseconds` and `gap_microseconds` are long enough to be comfortably detected at your data acquisition system's sampling rate.

Latency and jitter between an event in an application and an output pulse will depend mostly on delays within the host computer. Latency has not been characterized. 

### Test of jitter: 

* Used: 
    - a Teensy 3.2 with 96MHz clock
    - chromebook running linux.
    - commit 55cc48c of this repository
* Repeatedly send events using the included python script, sleeping for approximately 20ms in between. 
* After 5,000 recorded pulses:
    - Pulse width: 
        - min: 10.000ms
        - max: 10.004ms
        - std dev: 587ns
    - Gap between pulses: 
        - min: 10.173ms
        - max: 15.929ms
        - std dev: 92us

