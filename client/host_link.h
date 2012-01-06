#ifndef __HOST_LINK_H_
#define __HOST_LINK_H_

#include "timer.h"

////////////
//
// Forms a link with the host computer and exchanges frames
//

#define LINK_MAX_FRAME 64 //Maximum length of an unencoded frame
#define LINK_MAX_ENCFRAME (LINK_MAX_FRAME + 2)

//Op-codes for different frames
#define LINK_NOP        0 //No-op, can be sent or received
#define LINK_MOTOR_STAT 1 //Sent regularly, reporting the status of a motor
#define LINK_THERM_STAT 2 //Sent regularly, reporting the status of a thermistor/heater

#define LINK_MOTOR_PRGM 3 //Command to move motors using quadratic coefficients
#define LINK_THERM_PRGM 4 //Command to set a heater

#define LINK_PING       5 //May be sent by host, should receive back a pong
#define LINK_PONG       6 //Sent to host in response to a PING

typedef struct {
  uint8_t up : 1; //1 if there is a link to the host

  //For TX packet currently in flight
  uint8_t  tx_buf[LINK_MAX_ENCFRAME]; //The extra 2 is for cobs encoding overhead + zero byte
  uint8_t *tx_pos; //for tracking position in the tx buffer
  uint8_t tx_len;  //Length of data to send, nonzero if a tx is in progress
  
  //For reading in an RX packet
  uint8_t  rx_buf[LINK_MAX_ENCFRAME];
  uint8_t *rx_pos; //will be > rx_buf if a receive is in progress
  
  timer_t status_timer;
} host_link_cfg;

void init_host_link(host_link_cfg *link);

void host_link_tick(host_link_cfg *link);

#endif