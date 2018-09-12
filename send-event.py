#! /usr/bin/python3
import serial
from time import sleep

# This is an example of how to send event markers to the arduino from an application.

# This name will be different on different computers.
# Windows: something like COM1
# Linux: something like /dev/ttyACM0
# Mac: something like /dev/cu.usbmodem*
port_name = '/dev/ttyACM0'

# Must match the rate passed to Serial.begin() in the firmware
baud_rate = 115200

# How long to wait between sending events to the arduino.
seconds_between_events = 0.020 # 20 milliseconds

# This number will be represented in binary on the output channels.
# For example, sending 255 will cause all output channels to pulse at once.
# Sending powers of 2 will each cause a single channels to pulse.
event_number = 255


def init_serial(port_name, baud_rate):
    while True:
        try:
            port = serial.Serial(port_name, baud_rate, timeout=None)
        except serial.serialutil.SerialException as exception:
            print(exception)
            sleep(3)
        else:
            return port


def send_events(event_number, seconds_between_events, port_name, baud_rate):
    port = init_serial(port_name, baud_rate)
    while True:
    # for _ in range(repetitions):
        # print("sending:", event_number)
        try:
            port.write(bytes([event_number]))
        except serial.serialutil.SerialException as exception:
            print(exception)
            port.close()
            break
        sleep(seconds_between_events);
    port.close()


if __name__ == "__main__":
    send_events(event_number, seconds_between_events, port_name, baud_rate)
