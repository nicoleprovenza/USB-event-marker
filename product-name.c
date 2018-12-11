#include "usb_names.h"

// Override the Teensy 3.2's product name, so the computer can tell that this device is the
// USB Event Marker, and will know to send event codes to it (instead of
// accidentally sending them to some other unsuspecting device like your USB
// keyboard...)
//
// See discussion here: https://forum.pjrc.com/threads/23796-How-to-change-Teensy-3-0-PRODUCT_NAME

#define PRODUCT_NAME { 'U', 'S', 'B', 'E', 'v', 'e', 'n', 't', 'M', 'a', 'r', 'k', 'e', 'r' }
#define PRODUCT_NAME_LEN 14


struct usb_string_descriptor_struct usb_string_product_name = {
    2 + PRODUCT_NAME_LEN * 2,  // uint8_t bLength
    3,                         // uint8_t bDescriptorType
    PRODUCT_NAME               // uint16_t wString[]
};
