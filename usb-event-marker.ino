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

// The first 8 pins of PORTD, in order, on the Teensy 3.2 (and Teensy LC). These
// 8 pins correspond to the 8 bits of the value received over serial (except that some bits may be
// modified by the ALWAYS_PULSE_MASK). For example, sending a "1"
// will only pulse pin 2, and sending a "3" will pulse pin 2 and pin 14.
#define NUM_PINS 8
const uint8_t port_d_pins[NUM_PINS] = {2, 14, 7, 8, 6, 20, 21, 5};

// Force these bits/pins to pulse anytime we output an event code, regardless of
// the actual value of that bit in the event code. This is useful for eg. making
// an LED flash on every event. Since 1<<7 masks the most-significant bit,
// the max unique event code will be 127 instead of 255.
// To disable this feature and output all 8 bits of the event code, set this mask to 0x0.
#define ALWAYS_PULSE_MASK 0x1<<7


void setup() {
  // Set baud rate
  Serial.begin(9600);

  // Configure the output pins
  for (uint8_t i = 0; i < NUM_PINS; i++) {
    pinMode(port_d_pins[i], OUTPUT);
  }
}

// void debug_repeat() {
//   uint8_t event = Serial.read();
//   uint8_t fives = event/5;
//   uint8_t ones = event%5;
//   for(uint8_t i = 0; i < fives; i++) {
//     set(event | ALWAYS_PULSE_MASK);
//     delay(100);
//     clear();
//     delay(200);
//   }
//   for(uint8_t i = 0; i < ones; i++) {
//     set(event | ALWAYS_PULSE_MASK);
//     delay(10);
//     clear();
//     delay(200);
//   }
//   delay(500);
// }


void loop() {
  if (Serial.available()) {
    // An event happened!

    // debug_repeat();

    // Read 1 byte containing the event code and use it to set the
    // corresponding output pins.
    set(Serial.read() | ALWAYS_PULSE_MASK);

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
}

inline void clear() {
  // Clear all of the first 8 pins in the port.
  GPIOD_PCOR = 0xFF;

}
