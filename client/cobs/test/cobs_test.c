#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "usb_serial/usb_serial.h"
#include "timer.h"

#include "cobs.h"

#define LED_CONFIG (DDRD |= (1<<6))
#define LED_OFF    (PORTD &= ~(1<<6))
#define LED_ON     (PORTD |= (1<<6))
#define LED_TOGGLE (PIND = (1 << 6))

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

int main(void) {
  // set for 16 MHz clock
  CPU_PRESCALE(0);
  LED_CONFIG;
  LED_ON;

  //Initialize all the perhipherals
  usb_init();
  
  while(!usb_configured()) {
    LED_TOGGLE;
    _delay_ms(100);
  }
  
  while(1) for(uint8_t i=1; i <= 7; i++) {
    while (!(usb_serial_get_control() & USB_SERIAL_DTR));
    uint8_t buf[7] = { 0x02, 0x04, 0x00, 0x08, 0x10, 0x20, 0x01 };
    uint8_t out[16];
    uint16_t len;
    
    len = tecobs_stuff_frame(out, buf, i);
    //USB is running, look for a host
    if(usb_serial_write_available() > len) {
      LED_TOGGLE;
      _delay_ms(100);
      LED_TOGGLE;
      usb_serial_write(out, len);
    }
    _delay_ms(1000);
    LED_TOGGLE;
  }
}