#include <string.h>

#include "host_link.h"
#include "cobs.h"

#include "usb_serial/usb_serial.h"

//Helper functions called during tick_host_link()
static void do_receive_frame(host_link_state *restrict link);
static void do_transmit_frame(host_link_state *restrict cfg);

//some helper methods
//True if the host_link_state tx buffer is empty
static inline uint8_t tx_empty(host_link_state *link) {
  return link->tx_len == 0;
}
#define MIN(a,b) ((a) < (b) ? (a) : (b))



void setup_host_link(host_link_state *restrict link, const link_handlers_t *link_handlers) {
  //Initialize all the perhipherals
  usb_init();
  
  link->up = 0;
  link->cmd_tail  = 0;
  link->cmd_count = 0;
  link->user_handlers = link_handlers;
}

void tick_host_link(host_link_state *restrict link) {
  if(link->up) {
    //Take down the link if USB goes away
    if(!usb_configured()) {
      link->up = 0;
      return;
    }
    
    //Attempt to receive any frames
    do_receive_frame(link);
    do_transmit_frame(link);

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
 
static void do_receive_frame(host_link_state *restrict link) {
  uint8_t len, count;
  link_command *cmd;
  
  //Attempt to read a packet, if we don't already have one
  if(link->rx_pos == link->rx_buf || *(link->rx_pos - 1) != 0) {
    count = usb_serial_getpacket(link->rx_pos, link->rx_buf + LINK_MAX_ENCFRAME - link->rx_pos, 0);
    link->rx_pos += count;
  }
  //Attempt to queue the packet if there is one and there's space
  if(link->cmd_count < CMD_BUF_SIZE && (link->rx_pos > link->rx_buf) && *(link->rx_pos - 1) == 0) {
    len = tecobs_unstuff_frame(link->rx_buf, link->rx_pos - link->rx_buf);
    cmd = link->cmd_buf  + (link->cmd_tail + link->cmd_count) % CMD_BUF_SIZE;
    //TODO: Checksum check
    
    //Copy the command
    cmd->len = link->rx_pos - link->rx_buf;
    memcpy(&(cmd->opcode), link->rx_buf, cmd->len);
    link->cmd_count++;
    
    //Clear the rx_buf
    link->rx_pos = link->rx_buf;
  }
}

/***************
 * Logic to send and reset the USB buffer
 *
 *
 */
static uint8_t usb_write_helper(volatile uint8_t * const restrict dst, void * restrict state, const uint8_t maxlen) {
  host_link_state *cfg = (host_link_state*)state;
  uint8_t *start = cfg->tx_pos,
          *end = MIN(cfg->tx_buf + cfg->tx_len, cfg->tx_buf + maxlen);
  
  while(cfg->tx_pos < end) {
    *dst = *cfg->tx_pos++;
  }
  
  return cfg->tx_pos - start;
}

static void do_transmit_frame(host_link_state *restrict cfg) {
  //There's an active packet in the txbuffer
  if(cfg->tx_len > 0) {
    usb_serial_write_nb(usb_write_helper, cfg);

    //If we've fully sent the packet, clear the tx_buf
    if(cfg->tx_pos == (cfg->tx_buf + cfg->tx_len)) {
      cfg->tx_len = 0;
      cfg->tx_pos = cfg->tx_buf;
    }
  }
}

/*********
 * Queues a command for transmission
 *
 * @return true if the packet was queued, false if there was no room
 *
 */
uint8_t host_link_send(host_link_state *restrict link, const link_command *restrict cmd) {
  uint8_t len;
  if(!tx_empty(link)) return 0; //Cannot queue up if the txbuffer is full
  
  len = tecobs_stuff_frame(link->tx_buf, &(cmd->opcode), (cmd->data + cmd->len) - &cmd->opcode); //The +2 is for opcode and tid
  link->tx_len = len;
  link->tx_pos = link->tx_buf;
  
  //The tick_host_link(...) will actually transmit packet in the buffer
  
  return len;
}

/*******
 * Sends and packet currently in the TX buffer, then blocks until debug packet is sent
 * Returns immediately if the link is not down
 */
void host_link_send_debug(host_link_state *restrict link, void *restrict buf, uint8_t len) {
  link_command tx_cmd;
  while(link->up) {
    if(!tx_empty(link)) {
      tick_host_link(link);
      continue;
    }
    
    memcpy(tx_cmd.data, buf, len);
    tx_cmd.opcode = LINK_DEBUG;
    tx_cmd.tid    = 0;
    tx_cmd.len    = len;
    
    //Guaranteed to succeed since the txbuf is empty
    host_link_send(link, &tx_cmd);
    break;
  }
  //Wait for the txbuffer to fully transmit
  while(link->up && !tx_empty(link)) {
    tick_host_link(link);
  }
}

//Builtin functions
static uint8_t cmd_ping(struct _host_link_state *restrict link, uint8_t opcode, uint8_t tid, uint8_t *data, uint8_t len);



/*****
 * Handler table for builtin commands
 * 
 *
 */
static const link_handlers_t base_handlers PROGMEM = {
  //OP0 is a HELO/DEBUG
  .op1 = cmd_ping
};


/*******
 * Built-in function implementations
 *
 */
 
static uint8_t cmd_ping(struct _host_link_state *restrict link, uint8_t opcode, uint8_t tid, uint8_t *data, uint8_t len) {
  link_command cmd;
  
  cmd.opcode = opcode;
  cmd.tid    = tid;
  memcpy(cmd.data, data, len);
  return host_link_send(link, &cmd);
}
