#include "host_link.h"

#include "usb_serial/usb_serial.h"

static void transmit_frame(uint8_t *buf, uint8_t len);
static void do_receive_frame(host_link_cfg *restrict link);

void init_host_link(host_link_cfg *restrict link) {
  //Initialize all the perhipherals
  usb_init();
  
  link->up = 0;
}

void tick_host_link(host_link_cfg *restrict link) {
  uint8_t count;
  if(link->up) {
    //Take down the link if USB goes away
    if(!usb_configured()) {
      link->up = 0;
      return;
    }
    
    do_receive_frame(link);

    if(timer_fired(&link->status_timer)) {
      
    }
  } else {
    if(usb_configured()) {
      //USB is running, look for a host
      if(usb_serial_get_control() & USB_SERIAL_DTR) {
        link->up = 1;
        init_timer(&link->status_timer, 10000);
        
        //Reset the tx and rx buffers
        link->tx_pos = link->tx_buf;
        link->tx_len = 0;
        link->rx_pos = link->rx_buf;
      }
    }
  }
}

/**************
 *
 * Logic for receiving a frame and processing it
 *
 */
static void do_receive_frame(host_link_cfg *restrict link) {
  uint8_t len, count;
  //Attempt to read a packet
  count = usb_serial_getpacket(link->rx_pos, link->rx_buf + LINK_MAX_ENCFRAME - link->rx_pos, 0);
  link->rx_pos += count;
  if(count > 0 && *(link->rx_pos - 1) == 0) {
    //A complete frame!
    len = stecobs_unstuff_frame(link->rx_buf, link->rx_pos - link->rx_buf);
    //TODO: Checksum check
    
  }
 
}


/////////////
//
// Transmits a 
//
static void transmit_frame(uint8_t *buf, uint8_t len) {
  
}