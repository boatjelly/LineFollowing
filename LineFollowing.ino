/* Line Following v0.1
   By: Sasha Dauz, Robert J. Guziec, Jacob JM Horstman
   Written: April 19, 2025
   Edit:
   I/O Pins:
  A0: [INPUT] Left LF Sensor
  A1: [INPUT] Center LF Sensor
  A2: [INPUT] Right LF Sensor
  A3:
  A4:
  A5:
  D0:
  D1:
  D2:
  D3:
  D4:
  D5:
  D6:
  D7:
  D8:
  D9:
  D10:
  D11:
  D12:
  D13:
*/
void setup() {
  // Configure ADC
  ADCSRA = 0xE7; // [1110 0111]
                 // ADC Enabled               bit 7
                 // ADC Start Conversion      bit 6
                 // ADC Auto Trigger Enable   bit 5
                 // ADC Interrupt Flag        bit 4
                 // ADC Interrupt Disabled    bit 3
                 // ADC Prescaler set to 128  bits 210
                 
  ADCSRB = 0x05; // [0000 0101]
                 // -                         bits 7543
                 // Timer/counter 1 comp B    bits 210
                 
  ADMUX = 0x40;  // [0100 0000] -> A0 0x40
                 // [0100 0001] -> A1 0x41
                 // [0100 0010] -> A2 0x42
                 // Reference: AVCC
                 // ADLAR Full Precision (10-Bit) Mode

  // Configure TCNT1
  TCCR1A = 0x30;   // [0011 0000]
                   // OC1A disconnected      bits 76
                   // OC1B Set CompMatch     bits 54
                   // -                      bits 32
                   // CTC Mode               bits 10
                   
  TCCR1B = 0x05;   // [0000 1001]
                   // Input capture disabled bits 76
                   // -                      bit 5
                   // CTC Mode               bits 43
                   // Pre-scaler N=1         bits 210

  TCCR1C = 0x00;   // [0000 0000]
                   // Force compare disabled bits 76
                   // -                      bits 543210

   TIMSK1 = 0x40;  // [0000 0100]
                   // -                      bit 7643
                   // I/C interrupt Disabled bit 5
                   // Comp B Enabled         bit 2       <-- Requires a do nothing interrupt?
                   // Comp A Disabled        bit 1
                   // Ovf Disabled           bit 0
}

void loop() {
  // Do nothing?
}
