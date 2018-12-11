#! /usr/bin/python3
import serial
from time import sleep

# Our script for finding the serial port of the attached USB Event Marker
import find_event_marker_port


# This is an example of how to send event codes to the USB Event Marker device
# from an application. The event code will be represented in binary on the
# device's output pins. For example, sending 255 will cause all 8 output pins
# to pulse at once. Sending powers of 2 will each cause a single pin to
# pulse.
#
# Note: if the firmware's `ALWAYS_PULSE_MASK` feature is enabled, certain pins
# will always pulse on every event, regardless of the event code value (eg. to
# flash an LED on every event).

def send(port_file, event_code):
    """Write the event code (1-255) to the serial port."""
    print("sending:", event_code)
    port_file.write(bytes([event_code]))
    port_file.flush()

def loop_through_events(start_code, end_code, port_file):
    """Send all the events between start_code and end_code, with brief pauses in between"""
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

    # Close the file when we're done
    port_file.close()

if __name__ == "__main__":
    main()
