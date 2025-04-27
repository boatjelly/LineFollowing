/*
  Line Following Edge Cases
  Description: Jeremy doesn't like borderline values
  By: Sasha Dauz, Jacob Horstman, Robert Guziec
  Written: April 19, 2025
  Edited: April 24, 2025
  I/O
  A4:  [INPUT] Right Wheel Encoder
  A5:  [INPUT] Left Wheel Encoder
  ...
  D3:  [OCR2A] Right Motor Forward; Connects to Pin 15 on the H-Bridge
  D5:  [OCR0B] Left Motor Forward Speed; Connects to H-Bridge Pin 7
  D6:  [OCR0A] Left Motor Reverse Speed; Connects to H-Bridge Pin 2
  D11: [OCR2B] Right Motor Reverse Speed; Connects to Pin 10 on the H-Bridge

  **MAKE CORRECTION HERE BC OCR2A ACTUALLY CONTROLS RIGHT MOTOR FORWARD

  Using 9V battery

  Changes made:
  - Added case 3
  Jacob Changes:
  - Simplify ISR conditionals


*/

// * * * GROSS ENCODER GLOBALS * * *
volatile unsigned int avg = 0;

// * * * * GROSS GLOBAL VARIABLES * * * *
volatile unsigned char rightyTighty = 0;
volatile unsigned char leftyLoosey = 0;
volatile unsigned char center = 0;

// * * * SPEED CONSTANTS * * *
//const unsigned char FWD = 200;
const unsigned char TRN = 140; // threshold for both motors using battery packs
// New variables
const unsigned char EDGE_BLK = 230; // 230 Best number >:|
const unsigned char CENTER_BLK = 215;
const unsigned char BRDRLN = 115;

const unsigned int HLT = 1817;
const unsigned char LWHL = 255;  // Turns left (controls L wheel), use when right sensor detects black
const unsigned char RWHL = (220L * LWHL) / 255;  // Turns right (controls R wheel), use when left detects black

void setup() {
  // Configure serial monitor baudrate to 9600
  Serial.begin(9600);
  // Disable global interrupts
  cli();
  // Left Motor Setup
  TCCR0A = 0xA1;  // Configure Timer 0 for Phase-Correct PWM (Left motor)
  // 8-bit
  // TOP value 0xFF
  TCCR0B = 0x01;
  // Right Motor Setup
  TCCR2A = 0xA1;  // Configure Timer 2 for PWM (Right motor)
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
      // Collect LEFT sensor data A0
      leftyLoosey = ADCH;
      if (ADCH < EDGE_BLK) {
        OCR0B = LWHL;
        OCR0A = 0;
      } else {
        OCR0B = 0;
        OCR0A = TRN;
      }
      //Serial.println(ADCH < EDGE_BLK);
      ADMUX &= 0xF8;  // Selectively clear last three bits
      ADMUX |= 0x02;  // ADC read from A2
      sensor++;       // Go to case 1
      break;
    case 1:
      // Collect RIGHT sensor data A2
      rightyTighty = ADCH;
      if (rightyTighty < EDGE_BLK) {
        OCR2A = RWHL;
        OCR2B = 0;
      } else {
        OCR2A = 0;
        OCR2B = TRN;
      }
      ADMUX &= 0xF8;  // Selectively clear bits
      ADMUX |= 0x01; // Set to A1
      sensor++;
      break;

    // Handle edge cases
    case 2:
      // Detect if DRS should be enabled (go straight)
      if (((center >= EDGE_BLK) && ( leftyLoosey <= EDGE_BLK)) && (rightyTighty <= EDGE_BLK)) {
        OCR0B = LWHL;
        OCR2A = RWHL;
        OCR0A = 0;
        OCR2B = 0;
      }
      Serial.println(ADCH);
      ADMUX &= 0xF8;
      sensor = 0;  // Go back to case 0
      break;
  }

  // Start new conversion
  ADCSRA |= 0x40;

  //  // I have commitment issues. Keep this code for debugging.
  //  Serial.print("Left: ");
  //  Serial.println(leftyLoosey);
  //  Serial.print("Center: ");
  //  Serial.println(center);
  //  Serial.print("Right: ");
  //  Serial.println(rightyTighty);
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


/* Graveyard

   case 0:
      // Collect LEFT sensor data
      leftyLoosey = ADCH;
      // if left sensor reads white or right sees black, L wheel ON
      // otherwise (left sensor reads black), L wheel OFF
      if ((leftyLoosey < EDGE_BLK) || (rightyTighty >= EDGE_BLK)) {
        OCR0B = LWHL;
      } else {
        OCR0B = 0;
      }
      ADMUX &= 0xF8;  // Selectively clear last three bits
      ADMUX |= 0x01;  // ADC read from A1
      sensor++;       // Go to case 1
      break;
    case 1:
      // Collect CENTER sensor data
      center = ADCH;
      // If center sensor reads BLK or darker, full send
      // Duty cycles adjusted in global variables
      if (center >= CENTER_BLK) {
        OCR0B = LWHL;
        OCR2A = RWHL;
      }
      ADMUX &= 0xF8;  // Selectively clear bits
      ADMUX |= 0x02;  // ADC read from A2
      sensor++;       // Go to case 2
      break;
    case 2:
      // Collect RIGHT sensor data
      rightyTighty = ADCH;
      // if right sensor reads white or left sees BLK, R wheel ON
      // otherwise (right sensor reads black), R wheel OFF
      if ((rightyTighty < EDGE_BLK) || (leftyLoosey >= EDGE_BLK)) {
        OCR2A = RWHL;
      } else {
        OCR2A = 0;
      }
      ADMUX &= 0xF8;  // Selectively clear bits
      //sensor = 0;     // Go back to case 0
      sensor++;  // Go to case 3
      break;

    // Handle edge cases
    case 3:
      // Possible edge case: Right (50), Center (115), Left (244)
      if (((center <= CENTER_BLK) && (center >= BRDRLN)) && (leftyLoosey >= EDGE_BLK)) {
        OCR0B = LWHL;
      } else {
        OCR0B = 0;
      }

      if (((center <= CENTER_BLK) && (center >= BRDRLN)) && (rightyTighty >= EDGE_BLK)) {
        OCR2A = RWHL;
      } else {
        OCR2A = 0;
      }

      sensor = 0;  // Go back to case 0
      break;
*/
