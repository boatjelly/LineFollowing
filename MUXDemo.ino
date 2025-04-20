/*
  MUX Demo
  By: Sasha Dauz, Jacob Horstman
  Written: April 19, 2025
  I/O Pins
  A0: POT 0
  A1: POT 1
  ...
*/

void setup() {
  DDRD = 0x60;

  // Globally disable interrupts
  cli();
  // ADC in 8-bit mode, enable interrupts, auto trigger DISABLED
  ADCSRA = 0xCF;
  ADCSRB = 0x00;
  ADMUX = 0x60;
  // Timer/counter 0 fast PWM, N=64, OC0A and OC0B active
  TCCR0A = 0xA3;
  TCCR0B = 0x03;

  // Re enable interrupts
  sei();
}

void loop() {
  // Nothing to see here...
}

ISR(ADC_vect) {
  if (ADMUX & 0x01) {
    // A1 should control OC0B
    OCR0B = ADCH;
  } else {
    // A0 should control OC0A
    OCR0A = ADCH;
  }

  // Change ADMUX
  ADMUX ^= 1;

  // Start new conversion
  ADCSRA |= 0x40;
}