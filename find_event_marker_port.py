#! /usr/bin/python3

import sys
import subprocess as sp

from serial.tools.list_ports import comports

# Check whether the USB Event Marker device is plugged in to this linux computer.
#
# If it's connected to a serial port:
#  - configure the serial port
#  - print the path to the sysfs file representing the serial port
#  - exit with code 0
#
# If it's NOT connected:
#  - exit with a non-zero error code
#
# Other programs can run this script to get the path, then send event codes by
# writing bytes to the file. The path is printed to stdout, and all other
# error/status messages are printed to stderr.

def is_event_marker(port_info):
    """Check if the name of this usb device matches the one we set in product-name.c"""
    return port_info.product == 'USBEventMarker'

def eprint(message):
    """Print the message string to stderr, so it doesn't mess with the
    important output in stdout """
    print(message, file=sys.stderr)

def find_event_marker():
    """If the USB Event Marker is found, return info about it. If not, immediately
    exit the script with an error code."""
    ports = comports()
    matches = [p for p in ports if is_event_marker(p)]
    if not matches:
        eprint("Error: USB event marker not found")
        exit(1) # Fail
    if len(matches) > 1:
        eprint("Warning: Multiple USB event markers found. Picking one arbitrarily.")

    return matches[0].device  # Success!

def configure_port(port_path, baud_rate):
    """Configure the baud rate and other properties of the given serial port,
    so that other programs can simply write to it as if it were a normal file.
    If configuration fails, immediately exit the script with an error code.
    Requires the linux 'stty' utility. """

    # The path to the 'stty' utility. You can find its path on your computer by
    # running `$ which stty` in your terminal.
    stty = "/usr/bin/stty"

    # Use `raw` mode to disable any special behavior when sending numbers
    # that happen to be newline or carriage return characters.
    completed_process = sp.run(
        [
            stty,
            "-F", port_path,
            "raw",
            str(int(baud_rate))
        ],
        stdout=sp.PIPE, stderr=sp.PIPE
    )
    code = completed_process.returncode
    if code == 0:
        return # Success!

    # Non-zero code from the process means something unexpected or bad happened.
    # Print stty's error message, if there was one.
    stderr = completed_process.stderr
    if stderr is not None and stderr:
        eprint(stderr.decode('utf-8'))
    eprint("%s failed with exit code %d" % (stty, code))
    exit(1) # Fail

def find():
    eprint("Searching for USB Event Marker...")
    port_path = find_event_marker()
    eprint("Found USB Event Marker: '%s'" % port_path)

    eprint("Configuring serial port for USB Event Marker")

    # The baud rate here must match the baud rate passed to `Serial.begin()` in
    # the firmware. 9600 is a good value.
    configure_port(port_path, 9600)

    # Print the serial port's path to stdout, where it can be read by the
    # program calling this script.
    print(port_path)
    return port_path


if __name__ == '__main__':
    find()
