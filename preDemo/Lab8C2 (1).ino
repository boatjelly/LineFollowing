/*
  Lab 8, Circuit 2: Proportional control of light levels
  By: Robert J. Guizec and Jacob JM Horstman
  Written: March 26, 2025
  Edited: --
  I/O Pins
  A0: [INPUT] Photosensor
  A1:
  A2:
  A3:
  A4:
  A5:
  D0:
  D1:
  D2:
  D3:
  D4:
  D5: [OUTPUT] LED
  D6:
  D7:
  D8:
  D9:
  D10:
  D11:
  D12:
  D13:
*/
// define pV, sP, err
volatile unsigned char pV = 0;
const unsigned char sP = 123;
volatile int err = 0;

void setup() {
  cli();
  // Configure TCCR0A, TCCR0B, OCR0B
  TCCR0A = 0x23;
  TCCR0B = 0x03;
  TIMSK0 = 0x00;
  OCR0B = 0;
  // Assert D5 (OC0B) as an output pin
  DDRD = 0x20;
  // Configure ADC, 8-bit mode
  ADCSRA = 0xEF;
  ADCSRB = 0x00;
  ADMUX = 0x60;
  // Serial monitor baudrate = 9600
  Serial.begin(9600);
  // Re-enable interrupts
  sei();
}

void loop() {
  unsigned char kP = 2;
  OCR0B += (kP * err) / 10;
  Serial.print(pV);
  Serial.print('\t');
  Serial.println(sP);
}

volatile unsigned char storeREG = 0;
ISR(ADC_vect) {
  storeREG = SREG;
  pV = ADCH;
  err = sP - pV;
  SREG = storeREG;
}
