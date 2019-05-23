# USB Event Marker

Use an arduino-compatible `Teensy 3.2` board to output 8-channel digital event markers from a computer to a data acquisition system. When the board receives an event code, one or more of the 8 pins will output a short pulse simultaneously. The different combinations of 8 pins can encode 255 distinct event codes in binary, for identifying different types of events.

In the Cube, as of 12/2018, only the first 5 output pins are connected to OpenEphys analog input channels. That means that only the numbers in the range [1, 31] are valid event codes.

This repository contains:

- Firmware for the `Teensy 3.2` board, which will listen for events over the USB serial connection. 

- `find_event_marker_port.py`: A python script for finding the path to your USB Event Marker's serial port, configuring it using `stty`, and then printing the path to it. Other programs can run this script to get the path, and then writing single-byte event codes to it as if it was a normal binary file. 

- `example.py`:  An example python script for sending events to the USB Event Marker, by opening it as a binary file. 


## Installation

### Firmware

If you need to make a new USB Event Marker board or update the firmware of an existing one, follow these instructions.

1. Install the [Arduino IDE](https://www.arduino.cc/en/Main/Software). You must install the offline IDE version, instead of using the online "Arduino Create" tool.

2. Install the [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html) add-on to the Arduino IDE, to support the `Teensy 3.2` board.

3. Connect the device to the computer with a USB cable, if it wasn't already plugged in.

4. Launch the Arduino IDE. 

5. Open the file `usb-event-marker.ino`. 

6. In the menu bar, select `Tools -> Board: -> "Teensy 3.2/3.1"`. 

7. Select `Tools -> USB Type: -> "Serial + Keyboard + Mouse + Joystick"`.

8. Select `Tools -> CPU Speed: -> "96 MHz"`.

9. Go to `Tools -> Port` and select which port the device is connected to. The port name will vary, but might look something like `/dev/cu.usbmodem*` on a Mac, or `/dev/ttyACM*` in linux. You can check by unplugging the device and seeing which entry in the port list disappears.

10. Compile and upload the firmware by clicking the round arrow button, or by selecting `Sketch -> Upload` in the menu bar. Look for messages printed in the bottom part of the window. If the upload was successful, you can now use the reconfigured device. 

### Host Computer

The included `find_event_marker_port.py` script makes it simple to send events to the device from a host computer. It requires:

- linux
- the `stty` utility (should already be installed on Ubuntu)
- python3 (should already be installed on Ubuntu)
- the pyserial library (`$ pip3 install pyserial`)

If you cannot meet these requirements (eg. on Windows), you should be able to replicate the functionality of `find_event_marker_port.py` using other tools.

After installing the above requirements, add your username (`evan`) to the group (`dialout`) that's allowed to access serial ports:

    sudo usermod -a -G dialout evan

If this is the first time you are uploading the scipt to the teensy, copy the following file in the /etc/udev/rules.d/49-teensy.rules path.
https://www.pjrc.com/teensy/49-teensy.rules

Reboot the computer to let this change take effect.

Plug the USB Event Marker device into one of the computer's USB ports and try running the example script:

    python3 example.py

You should see the device's light flash every time an event code is sent. If you watch the device's outputs with an oscilloscope, OpenEphys, or other data acquisition system, you should see pulses encoding each event code's value in binary.

## Sending Events 

See `example.py` for a simple example of how to send event codes to the USB Event Marker device
from a python script.

To send events from your application in any other language:

 1. Run `find_event_marker_port.py` to configure the serial port and get the path to its file.
 2. Open the file as a writable, binary file.
 3. Write individual bytes (event codes) to the file.
 4. Close the file when your application is all finished running.

See these task repos for examples in [matlab](https://github.com/neuromotion/matlab-msit-task) and [javascript](https://github.com/neuromotion/resting-state-task).

## Firmware Configuration and Timing 

The firmware's `ALWAYS_PULSE_MASK` feature lets you force certain pins to always pulse on every event, regardless of the event code value. We use this to make an LED connected to the 8th pin flash whenever an event is received. That's useful for debugging and for synchronizing events with video, but limits the highest unique event code to 127, instead of 255. (Note that the highest event code will be limited even further if fewer than 7 of the output pins are connected to your data acquisition system, like in the 2018 Cube wiring.)

Try not to send two event markers that are closer together than the length of the pulse (10ms by default, controlled by `pulse_milliseconds` in the firmware). If you do, you'll see two full pulses with a very short gap between them (~10us by default, controlled by `gap_microseconds`). So it will look as though the second event happened right after the first pulse ended, even though in reality, it happened earlier than that.

Make sure that `pulse_milliseconds` and `gap_microseconds` are long enough to be comfortably detected at your data acquisition system's sample rate.

Latency and jitter between an event in an application and an output pulse will depend mostly on delays within the host computer. Latency has not been characterized. 

### Test of jitter: 

* Used: 
    - a Teensy 3.2 with 96MHz clock
    - a chromebook running linux
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

