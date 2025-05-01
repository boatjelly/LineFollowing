/*
  The Motors Are Working
  By: Sasha Dauz, Jacob Horstman, Robert Guziec
  Written: April 19, 2025
  Edited: April 24, 2025
  I/O
  A4:  [INPUT] Right Wheel Encoder
  A5:  [INPUT] Left Wheel Encoder
  ...
  D3:  [OCR2B] Right Motor Forward; Connects to Pin 15 on the H-Bridge
  D5:  [OCR0B] Left Motor Forward Speed; Connects to H-Bridge Pin 7
  D6:  [OCR0A] Left Motor Reverse Speed; Connects to H-Bridge Pin 2
  D11: [OCR2A] Right Motor Reverse Speed; Connects to Pin 10 on the H-Bridge
  Using battery packs + H-bridge PCB


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
const unsigned char TRN = 50;
const unsigned int HLT = 1817;

void setup() {
  // Disable global interrupts
  cli();
  // Configure serial monitor baudrate to 9600
  Serial.begin(9600);
  // Enable internal pull-up
  PORTC |= 0x30;  // A4, A5
  // Left Motor Setup
  TCCR0A = 0xA1;    // Configure Timer 0 for Phase-Correct PWM (Left motor)
  // 8-bit
  // TOP value 0xFF
  TCCR0B = 0x01;
  // Right Motor Setup
  TCCR2A = 0xA1;    // Configure Timer 2 for PWM (Right motor)
  // 8-bit
  // TOP value 0xFF
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
  // Re-enable global interrupts
  sei();
}

void loop() {
  // Nothing to see here...
}

// BLACK >> 255
// 0 << WHITE
ISR(ADC_vect) {
  // Cool local counter variable
  static unsigned char sensor = 0;
  switch (sensor) {
    case 0:
      // LEFT sensor
      leftyLoosey = ADCH;
      if (ADCH < BLK)
      {
        OCR0B = FWD;
      } else {
        OCR0B = 0;
      }
      ADMUX &= 0xF8; // Selectively clear last three bits
      ADMUX |= 0x01;
      //      Serial.print("Left:");
      //      Serial.println(leftyLoosey);
      //      Serial.print("Right:");
      //      Serial.println(rightyTighty);
      //      Serial.print("Center:");
      //      Serial.println(center);
      sensor++;
      if (avg >= HLT) {
        OCR0B = 0;
        OCR2A = 0;
      }
      break;
    case 1:
      // CENTER sensor
      center = ADCH;
      ADMUX &= 0xF8;
      ADMUX |= 0x02;
      //      Serial.print("Left:");
      //      Serial.println(leftyLoosey);
      //      Serial.print("Right:");
      //      Serial.println(rightyTighty);
      //      Serial.print("Center:");
      //      Serial.println(center);
      sensor++;
      if (avg >= HLT) {
        OCR0B = 0;
        OCR2A = 0;
      }
      break;
    case 2:
      // RIGHT sensor
      rightyTighty = ADCH;
      if (ADCH < BLK)
      {
        OCR2A = FWD;
      } else {
        OCR2A = 0;
      }
      ADMUX &= 0xF8;
      //      Serial.print("Left:");
      //      Serial.println(leftyLoosey);
      //      Serial.print("Right:");
      //      Serial.println(rightyTighty);
      //      Serial.print("Center:");
      //      Serial.println(center);
      sensor = 0;
      if (avg >= HLT) {
        OCR0B = 0;
        OCR2A = 0;
      }
      break;
  }
  // Start new conversion
  ADCSRA |= 0x40;
}

ISR(PCINT1_vect) {
  // Cool local variables!
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
