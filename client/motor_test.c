/* LED Blink Example with USB Debug Channel for Teensy USB Development Board
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2008 PJRC.COM, LLC
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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


///////
//
// Motor configuration block
//
static allegro_spi_state motor0_state;
static allegro_spi_state motor1_state;
static allegro_spi_state motor2_state;
static allegro_spi_state motor3_state;

const allegro_spi_cfg motor0_cfg PROGMEM = {
  .strobe_port = &PORTB,
  .strobe_mask = 0b00010000,
  .current_control = set_oc1b,
  .state = &motor0_state
};

const allegro_spi_cfg motor1_cfg PROGMEM = {
  .strobe_port = &PORTF,
  .strobe_mask = 0b00010000,
  .state = &motor1_state
};

const allegro_spi_cfg motor2_cfg PROGMEM = {
  .strobe_port = &PORTB,
  .strobe_mask = 0b00001000,
  .state = &motor2_state
};

const allegro_spi_cfg motor3_cfg PROGMEM = {
  .strobe_port = &PORTC,
  .strobe_mask = 0b01000000,
  .state = &motor3_state
};

/////////
//
// Host-link Configuration
//
//static host_link_cfg host_link;

#include "const_strings.h"
void send_pstr(const char* PROGMEM);

int main(void)
{
  timer_t led_timer, motor_timer, current_timer;
	// set for 16 MHz clock
	CPU_PRESCALE(0);
	LED_CONFIG;
	LED_ON;
	
  //We need interrupts for this!
  sei();

  setup_timer(); //Real-time clock

  setup_connector();
  if(is_miswired()) {
    init_timer(&led_timer, 1000);
    while(1)
      if(timer_fired(&led_timer)) LED_TOGGLE;
  }
  
  //init_host_link(&host_link);
  //init_timer(&led_timer, 250);
  
  wake_motors();

  setup_spi();
  init_timer(&led_timer, 5000);
  init_timer(&motor_timer, 1);
  init_timer(&current_timer, 20);
  
  setup_timer1_as_pwm();
  enable_oc1b();
  init_motor(&motor0_cfg);
  set_motor_current(&motor0_cfg, 30);
  // init_motor(&motor1_cfg);
  // init_motor(&motor2_cfg);
  // init_motor(&motor3_cfg);

  uint8_t current = 0;
  while(1) {
    if(timer_fired(&led_timer)) {
      LED_TOGGLE;
    }
    
    //tick_motor(&motor1_cfg);
    tick_motor(&motor0_cfg);

    if(timer_fired(&motor_timer)) {
      move_motor(&motor0_cfg, -1);
    }
    //if(timer_fired(&current_timer)) {
    //  set_motor_current(&motor0_cfg, current++);
    //}
  }
}

void send_pstr(const char* PROGMEM s)
{
	char c;
	while (1) {
		c = pgm_read_byte(s++);
		if (!c) break;
		usb_serial_putchar(c);
	}
}