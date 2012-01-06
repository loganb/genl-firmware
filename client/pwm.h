#ifndef __PWM_H_
#define __PWM_H_

/*******
 *
 * Configures timers 0,1,3,4 to be suitable for driving PWM output pins
 */
static inline void setup_timer0_as_pwm(void) {
  //no outputs, Fast PWM
  TCCR0A = 0b00000011;
  //no clock prescaler
  TCCR0B = 0b00000001;
}

static inline void setup_timer1_as_pwm(void) {
  TCCR1A = 0b00000001; //No outputs, FastPWM
  TCCR1B = 0b00001001; //FastPWM, no prescaler
}

// Timer 3 has no exposed pins on the Teensy 2.0
// static inline void setup_timer3_as_pwm(void) {
//   TCCR1A = 0b00000000; //No outputs, FastPWM
//   TCCR1B = 0b00001001; //FastPWM, no prescaler
// }

static inline void setup_timer4_as_pwm(void) {
  PLLFRQ &= 0b11001111; //Disable PLL clock for Timer4
  TCCR4A  = 0b00000000; //No outputs, no modulation
  TCCR4B  = 0b00000001; //No clock prescaler
  TCCR4C  = 0b00000000; //No outputs
  TCCR4D  = 0b00000000; //No fault protection, FastPWM
  TCCR4E  = 0b00000000; //No register lock, no enhanced mode
  
  TC4H  = 0;
  OCR4C = 0xFF; //Use timer4 as an 8-bit PWM
}

static inline void enable_oc1b(void) {
  TCCR1A |= 0b00100000; //Enable OC1B
  DDRB   |= 0b01000000;
  
  OCR1BH  = 0;
  OCR1BL  = 0; //Initialize to low
}

static inline void set_oc1b(uint8_t val) {
  OCR1BH = 0;
  OCR1BL = val;
}


/********
 *
 *
 */
 
#endif