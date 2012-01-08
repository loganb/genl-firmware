#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "spi.h"
#include "allegro.h"
#include "timer.h"
#include "util.h"
#include "connector.h"
#include "pwm.h"
#include "host_link.h"

#define LED_CONFIG (DDRD |= (1<<6))
#define LED_OFF    (PORTD &= ~(1<<6))
#define LED_ON     (PORTD |= (1<<6))
#define LED_TOGGLE (PIND = (1 << 6))

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

static const link_handlers_t myhandlers PROGMEM = {
  .op0 = 0,
};

static host_link_state host_link;


int main(void) {
  timer_t led_timer;
  // set for 16 MHz clock
	CPU_PRESCALE(0);
	LED_CONFIG;
	LED_ON;

  //We need interrupts for this!
  sei();
  setup_timer(); //Real-time clock
  setup_host_link(&host_link, &myhandlers);

  init_timer(&led_timer, 5000);

  unsigned long test_num = 1;
  while(1) {
    if(timer_fired(&led_timer)) {
      LED_TOGGLE;
      host_link_send_debug(&host_link, &test_num, sizeof(long));
      test_num <<= 1;
      if(test_num == 0) test_num = 1;
    }
    tick_host_link(&host_link);
  }
}