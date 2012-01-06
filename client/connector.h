
#include <avr/io.h>
#include <util/delay.h>

extern uint8_t reversed;
extern uint8_t miswired;

/**********************
 *
 * Detect if the connector is connected vagely correctly
 */
static inline void setup_connector(void) {
  DDRD  &= ~(1 << 4); //Configure D4 as input
  PORTD |=  (1 << 4);  //Enable D4 pullup
  
  DDRF  &= ~(1 << 5); //Configure F5 as input
  PORTF |=  (1 << 5); //Enble F5 pullup
  
  _delay_ms(25);
  
  if(bit_is_set(PIND, 4) && bit_is_clear(PINF, 5)) {
    reversed = 1;
    miswired = 1; //Reversed
  } else if(bit_is_clear(PIND, 4) && bit_is_set(PINF, 5)) {
    miswired = 0;
    reversed = 0; //Normal
    
    //Put drivers to sleep
    DDRF  |=  (1 << 5);
    PORTF &= ~(1 << 5);
  } else {
    miswired = 1; //Error
  }
}

static inline uint8_t is_miswired(void) {
  return miswired;
}

static inline void wake_motors(void) {
  PORTF |= (1 << 5);
}

static inline void sleep_motors(void) {
  PORTF &= ~(1 << 5);
}

static inline uint8_t is_awake(void) {
  return bit_is_set(PORTF, 5);
}