#! /usr/bin/python3
import serial
from time import sleep

import find_event_marker_port

# This is an example of how to send event markers to the arduino from an application.
# The event code will be represented in binary on the output channels.
# For example, sending 255 will cause all output channels to pulse at once.
# Sending powers of 2 will each cause a single channels to pulse.

def send(port_file, event_code):
    print("sending:", event_code)
    port_file.write(bytes([event_code]))
    port_file.flush()


def loop_through_events(start_code, end_code, port_file):
    seconds_between_events = 0.5
    print("Sending event codes %d-%d. Type ctrl-c to quit." % (start_code, end_code))
    for event_code in range(start_code, end_code+1):
        send(port_file, event_code)
        sleep(seconds_between_events);

def main():
    # Use the included script to find and configure the USB Event Marker serial port.
    # On linux, the path may be something like `/dev/ttyACM0`.
    port_path = find_event_marker_port.find()

    # We can open it as a writeable binary file
    port_file = open(port_path, 'wb')
    loop_through_events(1, 127, port_file)
    port_file.close()

if __name__ == "__main__":
    main()
