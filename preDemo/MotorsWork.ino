/*
  The Motors Are Working
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
  Using battery packs + H-bridge PCB
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
  // Configure OCRnB
  OCR0B = 255;       // Left motor forward
  OCR2B = 255;       // Right motor forward
}

void loop() {
  moveForward();
}
