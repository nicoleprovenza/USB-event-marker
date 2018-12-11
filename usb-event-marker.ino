// USB event marker
//
// Listen for single-byte event codes over serial, and immediately output their
// value in binary as a pulse across the output pins.
//
// Compatible with the Teensy 3.2 board.

// How long the pin(s) should stay high for after an event is received.
const uint16_t pulse_milliseconds = 10;

// How long to pause after ending a pulse. This matters if a second event
// is sent before the first pulse ends - it guarantees that you'll see a gap
// between those two pulses. Note that this value is just a minimum gap time,
// and the actual gap time will be ~6 microseconds longer.
const uint16_t gap_microseconds = 10;

// The first 8 pins of PORTD, in order, on the Teensy 3.2 (and Teensy LC). These
// 8 pins correspond to the 8 bits of the value received over serial (except that some bits may be
// modified by the ALWAYS_PULSE_MASK). For example, sending a "1"
// will only pulse pin 2, and sending a "3" will pulse pin 2 and pin 14.
#define NUM_PINS 8
const uint8_t port_d_pins[NUM_PINS] = {2, 14, 7, 8, 6, 20, 21, 5};

// Force these bits/pins to pulse anytime we output an event code, regardless of
// the actual value of that bit in the event code. This is useful for eg. making
// an LED flash on every event. Since 1<<7 masks the most-significant bit, the
// 8th pin will always pulse and the max unique event code will be 127 instead
// of 255. To disable this feature and output all 8 bits of the event code, set
// this mask to 0x0.
#define ALWAYS_PULSE_MASK 0x1<<7


void setup() {
  // Set baud rate for serial communication with the computer
  Serial.begin(9600);

  // Configure the output pins
  for (uint8_t i = 0; i < NUM_PINS; i++) {
    pinMode(port_d_pins[i], OUTPUT);
  }
}

void loop() {
  if (Serial.available()) {
    // An event happened!

    // Read 1 byte containing the event code and use it to set the
    // corresponding output pins.
    set(Serial.read() | ALWAYS_PULSE_MASK);

    // Let the pins stay high for the length of the pulse
    delay(pulse_milliseconds);

    // End the pulse on every pin
    clear();

    // Ensure there will be a small gap between the pulses of this event and the
    // next event, even if they were sent too close together.
    delayMicroseconds(gap_microseconds);
  }
}

inline void set(uint8_t value) {
  // Each bit in the value corresponds to one of the 8 pins.
  // If a bit is high, that output pin will be set high. If a bit is low, that
  // output pin will be unchanged. That means you can't use this function to set
  // output pins low - use clear() for that instead.
  GPIOD_PSOR = value;
}

inline void clear() {
  // Clear all 8 of the output pins, setting them low.
  GPIOD_PCOR = 0xFF;

}
