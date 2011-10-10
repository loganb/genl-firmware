
#include "util.h"

typedef struct {
  volatile uint8_t *pwm_register;
  volatile uint8_t *ddr_register;
  volatile uint8_t *port_register;
  uint8_t pin; //0-7
} heater;

static inline void init_heater(const heater *h) {
  volatile uint8_t mask, *ddr, *port;

  ddr  = WORD_FIELD(h, ddr_register);
  mask = 1 << BYTE_FIELD(h, pin);
  
  ddr |= mask;
  
}