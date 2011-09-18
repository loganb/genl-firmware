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

#include "spi_config.h"
#include "allegro.h"

#define LED_CONFIG	(DDRD |= (1<<6))
#define LED_ON		(PORTD &= ~(1<<6))
#define LED_OFF		(PORTD |= (1<<6))
#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))
#define DIT 80		/* unit time for morse code */

static allegro_spi_state motor0_state;
static allegro_spi_state motor1_state;
static allegro_spi_state motor2_state;
static allegro_spi_state motor3_state;

const allegro_spi_cfg motor0_cfg PROGMEM = {
  .strobe_port = &PORTB,
  .strobe_mask = 0b00010000,
  .pwm_register = 0, //TODO
  .state = &motor0_state
};


const allegro_spi_cfg motor1_cfg PROGMEM = {
  .strobe_port = &PORTF,
  .strobe_mask = 0b00010000,
  .pwm_register = 0, //TODO
  .state = &motor1_state
};

const allegro_spi_cfg motor2_cfg PROGMEM = {
  .strobe_port = &PORTB,
  .strobe_mask = 0b00001000,
  .pwm_register = 0, //TODO
  .state = &motor2_state
};

const allegro_spi_cfg motor3_cfg PROGMEM = {
  .strobe_port = &PORTC,
  .strobe_mask = 0b01000000,
  .pwm_register = 0, //TODO
  .state = &motor3_state
};

int main(void)
{
	unsigned char i, invert, drive;

	// set for 16 MHz clock, and make sure the LED is off
	CPU_PRESCALE(0);
	LED_CONFIG;
	LED_OFF;
	
  setup_spi();
  init_motor(&motor0_cfg);
  init_motor(&motor1_cfg);
  init_motor(&motor2_cfg);
  init_motor(&motor3_cfg);
}

