/*
  The Motors Are Working
  By: Sasha Dauz, Jacob JM Horstman, Robert Guziec
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
  ... But can you count?
*/

// * * * GROSS ENCODER GLOBALS * * *
volatile unsigned int avg = 0;
const unsigned int DISTTICK = 1887; // 2 meters / 1.06 mm/toggler = 1,886.79 toggles

// * * * * GROSS GLOBAL VARIABLES * * * *
volatile unsigned char rightyTighty = 0;
volatile unsigned char leftyLoosey = 0;
volatile unsigned char center = 0;

// * * * SPEED CONSTANTS * * *
const unsigned char FWD = 255;
const unsigned char BLK = 215;
const unsigned char SPED1 = 200;
const unsigned char SPED2 = 20;

void setup() {
  // Disable global interrupts
  cli();
  // Configure serial monitor baudrate to 9600
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
  // Enable interal pull-up
  PORTC |= 0x30;  // A4, A5
  // Re-enable global interrupts
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
      if ((ADCH < BLK) && (avg < DISTTICK))
      {
        OCR0B = FWD;
      } else {
        OCR0B =0;
      }
      ADMUX &= 0xF8; // Selectively clear last three bits
      ADMUX |= 0x01;
      sensor++;
      break;
    case 1:
      // CENTER sensor
      center = ADCH;
      ADMUX &= 0xF8;
      ADMUX |= 0x02;
      sensor++;
      break;
    case 2:
      // RIGHT sensor
      if ((ADCH < BLK) && (avg < DISTTICK))
      {
        OCR2A = FWD;
      } else {
        OCR2A =0;
      }
      ADMUX &= 0xF8;
      sensor++;
      break;
    default:
      sensor = 0;
      break;
  }
  // Start new conversion
  ADCSRA |= 0x40;
}

ISR(PCINT1_vect) {
  
  // Cool local variables!
  volatile unsigned int rightA = 1;
  volatile unsigned int rightB = 1;
  volatile unsigned int leftA = 1;
  volatile unsigned int leftB = 1;
  volatile unsigned int counterRight = 0;
  volatile unsigned int counterLeft = 0;

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
  avg = (counterLeft + counterRight)/2;
}
