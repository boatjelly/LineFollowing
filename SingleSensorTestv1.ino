/*
  Single Sensor Test
  By: Sasha Dauz, Jacob Horstman
  Written: April 19, 2025
  I/O Pins
  A1: [Input] Center Line Follower   
  D8: [Output] LEDs (all 4 of em)
  Comments: Black = 0
            White = 1023
  ...
*/

void setup() {
  Serial.begin(9600);

  // Configure Port B output
  DDRB = 0x01;

  // Configure ADC
    ADCSRA = 0xE7; // [1110 0111]
                  // ADC Enabled               bit 1
                  // ADC Start Conversion      bit 2
                  // ADC Auto Trigger Enable   bit 3
                  // ADC Interrupt Flag        bit 4
                  // ADC Interrupt Disabled    bit 5
                  // ADC Prescaler set to 128  bits 678
                  
    ADCSRB = 0x00; // [0000 0000]
                  // -                         bits 7543
                  // Free-running mode         bits 210
                  
    ADMUX = 0x41;  // [0100 0000] A0 | [0100 0001] A1 | [0100 0010] A2
                  // Reference AVCC
                  // ADLAR = 0 (Full Prec)
                  // ADC0 0x40, ADC1 0x41, ADC2 0x42
}

void loop() {
  PORTB |= 0x01;
  Serial.print("ADC Value" );
  Serial.println(ADC);
}
