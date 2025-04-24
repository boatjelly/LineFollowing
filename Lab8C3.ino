/*
  Lab 8, Circuit 3: Proportional-integral control of light levels
  By: Robert J. Guizec and Jacob JM Horstman
  Written: March 27, 2025
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
// define pV, sP, err, tau, errHist[]
volatile unsigned char pV = 0;
const unsigned char sP = 123;
volatile int err = 0;
const unsigned char tau = 255;
volatile int errHist[tau] = {};

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
  unsigned char kI = 1;  // kI < kP
  int histSum = sum(errHist, tau);
  OCR0B += (kP * err) / 10;
  OCR0B += (kI * histSum) / 100;
  Serial.print(pV);
  Serial.print('\t');
  Serial.println(sP);
}

volatile unsigned char storeREG;
ISR(ADC_vect) {
  storeREG = SREG;
  static int x = 0;
  pV = ADCH;
  err = sP - pV;
  errHist[x] = err;
  x++;
  if ( x == tau ) {
    x = 0;
  }
  SREG = storeREG;
}

int sum(volatile signed int * errArray, unsigned char t) {
  int tempVar = 0;
  for ( unsigned char j = 0; j < t; j++) {
    tempVar += errArray[j];
  }
  return tempVar;
}
