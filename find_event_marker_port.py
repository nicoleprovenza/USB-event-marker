#! /usr/bin/python3

import sys
import subprocess as sp

from serial.tools.list_ports import comports

def is_event_marker(port_info):
    return port_info.product == 'USBEventMarker'

def eprint(message):
    '''Print the message string to stderr, so it doesn't mess with the
    important output in stdout'''
    print(message, file=sys.stderr)

def find_event_marker():
    ports = comports()
    matches = [p for p in ports if is_event_marker(p)]
    if not matches:
        eprint("Error: USB event marker not found")
        exit(1) # failure
    if len(matches) > 1:
        eprint("Warning: Multiple USB event markers found. Picking one arbitrarily.")

    return matches[0].device   # success

def configure_port(port_path, baud_rate):
    '''Configure the baud rate of the given serial port, so that other programs
    can simply write to it as if it were a normal file. Use `raw` setting to
    disable any special behavior when sending numbers that happen to be newline
    or carriage return characters. Requires the linux "stty" utility.'''
    stty = "/usr/bin/stty"
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
        # Success!
        return

    # Non-zero code from the process means something unexpected or bad happened
    stderr = completed_process.stderr
    if stderr is not None and stderr:
        eprint(stderr.decode('utf-8'))
    eprint("%s failed with exit code %d" % (stty, code))
    exit(1)

def find():
    eprint("Searching for USB Event Marker...")
    port_path = find_event_marker()
    eprint("Found USB Event Marker: '%s'" % port_path)

    eprint("Configuring serial port for USB Event Marker")
    configure_port(port_path, 9600)

    # Print the serial port's path to stdout, where it can be read by the
    # program calling this script.
    print(port_path)
    return port_path


if __name__ == '__main__':
    find()
