// USB event marker
//
// Listen for single-byte event codes over serial, and immediately output their
// value in binary as a pulse across the output pins.
//
// Compatible with the Teensy LC and Teensy 3.2 boards.

// How long the pin(s) should stay high for after an event is received.
const uint16_t pulse_milliseconds = 10;

// How long to pause after ending a pulse. This matters if a second event
// is sent before the first pulse ends - it guarantees that you'll see a gap
// between those two pulses. Note that this value is just a minimum gap time,
// and the actual gap time will be ~6 microseconds longer.
const uint16_t gap_microseconds = 10;

// The first 8 pins of PORTD, in order (on the Teensy LC / 3.2) These 8 pins
// correspond to the 8 bits of the value received over serial. For example,
// sending a "1" will only pulse pin 2, and sending a "128" will only pulse pin
// 5. Sending a "255" will pulse all pins at once.
#define NUM_PINS 8
const uint8_t port_d_pins[NUM_PINS] = {2, 14, 7, 8, 6, 20, 21, 5};

// Uncomment the next line to blink the LED when an event is received.
// This might affect the marker timing very slightly.
#define DEBUG_BLINK

// The on-board LED
const uint8_t led_pin = 13;

void setup() {
  // Set baud rate
  Serial.begin(115200);

  // Configure the output pins
  for (uint8_t i = 0; i < NUM_PINS; i++) {
    pinMode(port_d_pins[i], OUTPUT);
  }

#ifdef DEBUG_BLINK
  pinMode(led_pin, OUTPUT);
#endif
}

void loop() {
  if (Serial.available()) {
    // An event happened!

    // Read 1 byte containing the event code and use it to set the
    // corresponding output pins
    set(Serial.read());

    // Let the pins stay high for the length of the pulse
    delay(pulse_milliseconds);

    // End the pulse on every pin
    clear();

    // Ensure there will be a small gap between the pulses of events that were
    // sent too close together.
    delayMicroseconds(gap_microseconds);
  }
}

inline void set(uint8_t value) {
  // Each bit in the value corresponds to one of the first 8 pins in the port.
  // This will set output high for any bit that is high. Bits that are low will
  // not clear pins that were already high! You should call clear() first to do
  // that.
  GPIOD_PSOR = value;

#ifdef DEBUG_BLINK
  digitalWrite(led_pin, HIGH);
#endif
}

inline void clear() {
  // Clear all of the first 8 pins in the port.
  GPIOD_PCOR = 0xFF;

#ifdef DEBUG_BLINK
  digitalWrite(led_pin, LOW);
#endif
}
