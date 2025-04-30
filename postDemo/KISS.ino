/*
  K.I.S.S.
  By: Sasha Dauz, Jacob Horstman, Robert Guziec
  Written: April 29, 2025
  Edited: Arpil 30, 2025
  I/O
  A4:  [INPUT] Right Wheel Encoder
  A5:  [INPUT] Left Wheel Encoder
  ...
  D3:  [OCR2A] Right Motor Forward; Connects to Pin 15 on the H-Bridge
  D5:  [OCR0B] Left Motor Forward Speed; Connects to H-Bridge Pin 7
  D6:  [OCR0A] Left Motor Reverse Speed; Connects to H-Bridge Pin 2
  D11: [OCR2B] Right Motor Reverse Speed; Connects to Pin 10 on the H-Bridge
*/

// * * * ENCODER GLOBAL VARIABLE * * *
volatile unsigned int avg = 0;

// * * * * GLOBAL VARIABLES * * * *
volatile unsigned char rightyTighty = 0;
volatile unsigned char leftyLoosey = 0;
volatile unsigned char center = 0;

void setup() {
  cli();
  // Configure TCNTn for each motor
  TCCR0A = 0xA1; // Left Motor Setup
  TCCR0B = 0x01;
  TCCR2A = 0xA1; // Right Motor Setup
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
  // Enable internal pull-up
  PORTC |= 0x30;  // A4, A5
  sei();
}

// * * * CONSTANTS * * *
const unsigned char FWD = 230;
const unsigned char RVS = 60;
const unsigned char BLK = 215;
const unsigned int HLT = 2000;

void loop() {
    if ( avg <= HLT) {
  while (( leftyLoosey < BLK) && (rightyTighty < BLK) && (center >= BLK)) {
    OCR0B = FWD;    // Left FWD
    OCR2A = FWD;    // Right FWD
    OCR0A = 0;      // Left BCK
    OCR2B = 0;      // Right BCK
  } // Send it
  while (leftyLoosey < BLK) {
    OCR0B = FWD;    // Left FWD
    OCR2A = 0;      // Right FWD
    OCR0A = 0;      // Left BCK
    OCR2B = RVS;      // Right BCK
  } // This^ should turn right
  while (rightyTighty < BLK) {
    OCR0B = 0;      // Left FWD
    OCR2A = FWD;    // Right FWD
    OCR0A = RVS;      // Left BCK
    OCR2B = 0;      // Right BCK
  } // This^ should turn left
}
else {
  OCR0B = 0;      // Left FWD
  OCR2A = 0;      // Right FWD
  OCR0A = 0;      // Left BCK
  OCR2B = 0;      // Right BCK
}
}

ISR(ADC_vect) {
  static unsigned char sensor = 0;
  switch (sensor) {
    case 0: // Collect LEFT sensor data A0, then change to A2
      leftyLoosey = ADCH;
      ADMUX &= 0xF8;
      ADMUX |= 0x02;
      sensor++;
      break;
    case 1: // Collect RIGHT sensor data A2, then change to A1
      rightyTighty = ADCH;
      ADMUX &= 0xF8;
      ADMUX |= 0x01;
      sensor++;
      break;
    case 2: // Collect Center sensor data A1, then change to A0
      center = ADCH;
      ADMUX &= 0xF8;
      sensor = 0;
      break;
  }
  ADCSRA |= 0x40;
}

ISR(PCINT1_vect) {
  static unsigned char rightA = 1;
  static unsigned char rightB = 1;
  static unsigned char leftA = 1;
  static unsigned char leftB = 1;
  static unsigned int counterRight = 0;
  static unsigned int counterLeft = 0;
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

  if (avg == 65535) {
    avg = 0;
  } // Prevents int overflow
}
