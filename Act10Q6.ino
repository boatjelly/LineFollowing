/*
  Activity 10 Q6
  By: Sasha Dauz, Jacob Horstman, Robert Guziec
  Written: March 17, 2025
  Edited: April 18, 2025
  I/O
  A4: [Input] Right Wheel Encoder
  A5: [Input] Left Wheel Encoder
*/
volatile unsigned long counterRight = 0;
volatile unsigned long counterLeft = 0;
volatile unsigned long avg = 0;
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
  // Enable interal pull-up
  PORTC |= 0x30;  // A4, A5
  // Re-enable global interrupts
  sei();
}

void loop() {
  Serial.print("Left (mm): ");
  Serial.println((counterLeft * 106) / 100);
  Serial.print("Right (mm): ");
  Serial.println((counterRight * 106) / 100);
  Serial.print("Avg (mm): ");
  Serial.println((avg * 106) / 100);
  _delay_ms(1000);
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
  avg = (counterLeft + counterRight)/2;
}