/*
  ~~~1 Step at a time~~~
  By: Sasha Dauz, Jacob Horstman
  Written: April 19, 2025
  Edited: April 20, 2025
  I/O
  A4:  [INPUT] Right Wheel Encoder
  A5:  [INPUT] Left Wheel Encoder
  ...
  D3:  [OCR2B] Right Motor Forward; Connects to Pin 15 on the H-Bridge
  D5:  [OCR0B] Left Motor Forward Speed; Connects to H-Bridge Pin 7
  D6:  [OCR0A] Left Motor Reverse Speed; Connects to H-Bridge Pin 2
  D11: [OCR2A] Right Motor Reverse Speed; Connects to Pin 10 on the H-Bridge
*/
volatile unsigned char counterRight = 0;
volatile unsigned char counterLeft = 0;
volatile unsigned char avg = 0;
volatile unsigned char rightA = 1;
volatile unsigned char rightB = 1;
volatile unsigned char leftA = 1;
volatile unsigned char leftB = 1;

void setup() {
  // Disable global interrupts
  cli();
  // Configure serial monitor baudrate to 9600
  Serial.begin(9600);
  // Configure Pin Change Interrupts
  PCICR = 0x02;   // PortC
  PCMSK1 = 0x30;  // A4, A5
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

  // Re-enable global interrupts
  sei();
}

void moveForward() {
  counterLeft() = 0;
  counterRight() = 0;

  // Configure OCRnB
  OCR0B = 237;       // Left motor forward
  OCR2B = 255;       // Right motor forward
  while (avg != 192)
    ;
  // Clear OCRnB
  OCR0B = 0;         // Clear left motor
  OCR2B = 0;         // Clear right motor
}

void loop() {
  moveForward();
}

ISR(PCINT1_vect) {
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
  avg = (counterLeft + counterRight) >> 1; // avg range [0,192]
}

/*
 * 
 *   // Reset counters after one revolution
  // Hopefully only an unsigned char will be needed to store
  if (counterRight == 193) {
    counterRight = 0;
  }
  if (counterLeft == 193) {
    counterLeft = 0;
  }
 */
