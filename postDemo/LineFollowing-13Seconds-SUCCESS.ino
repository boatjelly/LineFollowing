/*
  Demonstration of +13 second track time
  By: Sasha Dauz, Jacob Horstman, Robert Guziec
  Written: April 19, 2025
  Edited: May 1, 2025
  I/O:
  A0:  [INPUT] Left Line Sensor
  A1:  [INPUT] Center Line Sensor
  A2:  [INPUT] Right Line Sensor
  A4:  [INPUT] Right Wheel Encoder
  A5:  [INPUT] Left Wheel Encoder
  ...
  D3:  [OCR2B] Right Motor Reverse; Connects to Pin 15 on the H-Bridge
  D5:  [OCR0B] Left Motor Forward Speed; Connects to H-Bridge Pin 7
  D6:  [OCR0A] Left Motor Reverse Speed; Connects to H-Bridge Pin 2
  D11: [OCR2A] Right Motor Forward Speed; Connects to Pin 10 on the H-Bridge

  Proud of you Jer!
*/

// * * * GROSS ENCODER GLOBALS * * *
volatile unsigned int avg = 0;

// * * * * GROSS GLOBAL VARIABLES * * * *
volatile unsigned char rightyTighty = 0;
volatile unsigned char leftyLoosey = 0;
volatile unsigned char center = 0;

// * * * SPEED CONSTANTS * * *
const unsigned char FWD = 230;
const unsigned char BLK = 215;
const unsigned int HLT = 1817;

void setup() {
  // Disable global interrupts
  cli();
  // Enable internal pull-up
  PORTC |= 0x30;  // A4, A5
  // Left Motor Setup
  TCCR0A = 0xA1;
  TCCR0B = 0x01;
  // Right Motor Setup
  TCCR2A = 0xA1;
  TCCR2B = 0x01;
  // Configure output pins
  DDRD = 0x68;  // D3, D5, D6 as output for motors
  DDRB = 0x08;  // D11 as output for motor control
  // ADC in 8-bit mode, enable interrupts, auto trigger DISABLED
  ADCSRA = 0xCF;
  ADCSRB = 0x00;
  ADMUX = 0x60;
  // Configure Pin Change Interrupts
  PCICR = 0x02;   // PortC
  PCMSK1 = 0x30;  // A4, A5
  // Re-enable global interrupts
  sei();
}

void loop() {
  // Nothing to see here...
}

ISR(ADC_vect) {
  static unsigned char sensor = 0;
  switch (sensor) {
    case 0: // LEFT sensor
      leftyLoosey = ADCH;
      if (ADCH < BLK) {
        OCR0B = FWD;
      } else {
        OCR0B = 0;
      }
      ADMUX &= 0xF8;
      ADMUX |= 0x01;
      sensor++;
      if (avg >= HLT) {
        OCR0B = 0;
        OCR2A = 0;
      }
      break;
    case 1: // CENTER sensor
      center = ADCH;
      ADMUX &= 0xF8;
      ADMUX |= 0x02;
      sensor++;
      if (avg >= HLT) {
        OCR0B = 0;
        OCR2A = 0;
      }
      break;
    case 2: // RIGHT sensor
      rightyTighty = ADCH;
      if (ADCH < BLK) {
        OCR2A = FWD;
      } else {
        OCR2A = 0;
      }
      ADMUX &= 0xF8;
      sensor = 0;
      if (avg >= HLT) {
        OCR0B = 0;
        OCR2A = 0;
      }
      break;
  }
  ADCSRA |= 0x40; // Start new conversion
}

ISR(PCINT1_vect) {
  static unsigned int rightA = 1;
  static unsigned int rightB = 1;
  static unsigned int leftA = 1;
  static unsigned int leftB = 1;
  static unsigned int counterRight = 0;
  static unsigned int counterLeft = 0;
  // Calculate encoder values
  rightB = (PINC & 0x10) >> 4;
  leftB = (PINC & 0x20) >> 5;
  if ((rightB != rightA) && !rightB) {
    counterRight++;
  }
  if ((leftB != leftA) && !leftB) {
    counterLeft++;
  }
  leftA = leftB;
  rightA = rightB;
  avg = (counterLeft + counterRight) / 2;
}
