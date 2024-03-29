
/*
  ATMega328 - Testing use of 16-bit timer1 to generate a 20kHz PWM carrier with decent duty cycle resolution
  
  Note - some arduino functions might not play nice with this code (e.g. millis, analogWrite)
*/


// This is where we'll store the PWM cycle counter upper limit (in 16MHz ticks)
unsigned int pwmCycle = 0;

// Set the frequency in Hz of the PWM carrier. 
// Must be > ~300Hz and < ~16MHz
void setCarrier(unsigned long hz)
{
  // ICR1 is the length of the PWM carrier cycle
  // measured in 16MHz counts
  pwmCycle = (float)16000000/hz;
  ICR1H = highByte(pwmCycle); 
  ICR1L = lowByte(pwmCycle);   
}

// Set the duty cycle. This is the count threshold at which the output goes from
// HIGH to LOW on each PWM carrier cycle
// Set the duty cycle. This is the count threshold at which the output goes from
// HIGH to LOW on each PWM carrier cycle
void setDuty(float percent)
{
  // Convert the % into real counter value
  long threshold = (fabs(percent)/100.0) * pwmCycle;
  
  // Driving forwards... will output on digital pin 10
  if(percent < 0.0)
  {
    if(percent <= -100.0) // full duty 
    {
      TCCR1A &= 0b00001111; // disconnect PWM, normal GPIO on PB1 and PB2
      PORTB &= 0b11111101; // pin 9 LOW
      PORTB |= 0b00000100; // pin 10 HIGH
    }
    else
    {
      OCR1BH = highByte(threshold); 
      OCR1BL = lowByte(threshold);   
      TCCR1A |= 0b00100000; // connect PWM to pin 10
      PORTB &= 0b11111001; // pin 9,10 LOW
    }
  }
  // Driving reverse... will output on digital pin 9
  else if(percent > 0.0)
  {
    if(percent >= 100.0) // full duty 
    {
      TCCR1A &= 0b00001111; // disconnect PWM, normal GPIO on PB1 and PB2
      PORTB &= 0b11111011; // pin 10 LOW
      PORTB |= 0b00000010; // pin 9 HIGH
    }
    else
    {
      OCR1AH = highByte(threshold); 
      OCR1AL = lowByte(threshold);   
      TCCR1A |= 0b10000000; // connect PWM to pin 9
      PORTB &= 0b11111001; // pin 9,10 LOW
    }
  }
  else
  {
    // if the duty is zero, just disengage PWM and set outputs LOW
    TCCR1A &= 0b00001111; // disconnect PWM, normal GPIO on PB1 and PB2
    PORTB &= 0b11111001; // pin 9,10 LOW
  }
}

void setup()
{
  pinMode(10, OUTPUT); 
  pinMode(9, OUTPUT); 

  // Configure timer 1 PWM
  // WGM13:WGM10   b1111   Fast PWM mode, cycle duration set by OCR1A
  // CS12:CS10     0b001    1:1 prescaler (counting at 16Mhz)
  //  TCCR1A = _BV(WGM11) | _BV(WGM10);  
  TCCR1A = _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);
  setCarrier(14000UL);
  setDuty(-50.0);
  Serial.begin(9600); 
}

void loop()
{
  
  
  
 // motorPower = sin(millis() / 1000.0f / PI) * 100.0f;
  //setDuty( sin(millis() / 1000.0f / PI) * 90.0f); 

  Serial.println(sin(millis() / 1000.0f / PI) * 90.0f);
}

