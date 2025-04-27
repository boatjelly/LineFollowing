/*
  Line Following
  By: Sasha Dauz, Jacob Horstman
  Written: April 19, 2025
  I/O Pins
  A0: [Input] Left Line Sensor
  A1: [Input] Center Line Sensor
  A2: [Input] Right Line Sensor
  ...
  D5: [Output] Center Line Sensor LED OC0B
  D6: [Output] Right Line Sensor LED OC0A
  D11: [Output] Left Line Sensor LED OC2A
  ...
*/

void setup() {
  Serial.begin(9600);
  // Set outputs on Port D and Port B
  DDRD = 0x60; // D6, D5
  DDRB = 0x08; // D11
  // Globally disable interrupts
  cli();
  // ADC in 8-bit mode, enable interrupts, auto trigger DISABLED
  ADCSRA = 0xCF;
  ADCSRB = 0x00;
  ADMUX = 0x60;
  // Timer/counter 0 fast PWM, N=64, OC0A and OC0B active
  TCCR0A = 0xA3;
  TCCR0B = 0x03;
  // Timer/counter 2
  TCCR2A = 0x83;
  TCCR2B = 0x03;
  // Re enable interrupts
  sei();
}

void loop() {
  // Nothing to see here...
}

// Global counter variable
volatile unsigned char counter = 0;
ISR(ADC_vect) {
  if (ADMUX & 0x02) {
    // A2 should control OC0A
    OCR0A = ADCH;
    ADMUX &= 0xF8;
    Serial.print("Right: ");
    Serial.println(ADCH);
  } else if (ADMUX & 0x01) {
    // A1 should control OC0B
    OCR0B = ADCH;
    ADMUX &= 0xF8;
    ADMUX |= 0x02;
    Serial.print("Left: ");
    Serial.println(ADCH);
  } else {
    // A0 should control OC2A
    OCR2A = ADCH;
    ADMUX &= 0xF8; // Selectively clear last three bits
    ADMUX |= 0x01;
    Serial.print("Center: ");
    Serial.println(ADCH);
  }
  // Start new conversion
  ADCSRA |= 0x40;
}
