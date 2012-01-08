#ifndef __HOST_LINK_H_
#define __HOST_LINK_H_

#include <stdint.h>
#include <avr/pgmspace.h>

#include "timer.h"

////////////
//
// Forms a link with the host computer and exchanges frames
//

#define LINK_MAX_FRAME ((uint8_t)64) //Maximum length of an unencoded frame
#define LINK_MAX_ENCFRAME ((uint8_t)(LINK_MAX_FRAME + 2))
#define CMD_BUF_SIZE ((uint8_t)2)

//Op-codes for different commands
#define LINK_NOP        0 //No-op, can be sent or received
#define LINK_DEBUG      1 //Client can send these with debug data
#define LINK_PING       2 //May be sent by host, should receive back a pong
#define LINK_PONG       3 //Sent to host in response to a PING

#define LINK_MOTOR_STAT 10 //Sent regularly, reporting the status of a motor
#define LINK_MOTOR_PRGM 11 //Command to move motors using quadratic coefficients

#define LINK_THERM_STAT 20 //Sent regularly, reporting the status of a thermistor/heater
#define LINK_THERM_PRGM 21 //Command to set a heater

struct _host_link_state;
struct _link_handlers_t;
//
// An array of 128 of these in PROGMEM should be passed to host_link_tick
// The 0th handler in the array is for opcode 0, and so on
// Opcodes 128 - 255 are for built-in use
//
typedef uint8_t(*link_command_f)(struct _host_link_state *restrict link, uint8_t opcode, uint8_t tid, uint8_t *data, uint8_t len);

typedef struct {
  uint8_t len;       //Lenth of the "data" field
  //Data is written into the struct starting at the opcode field, so the struct also represents the layout of commands
  uint8_t opcode;    //Opcode of the command
  uint8_t tid;       //Transaction id of the command
  uint8_t data[LINK_MAX_FRAME - 2]; //command-specific data
} link_command;

/*******
 * Holds all the state associated with the link to the host
 */
typedef struct _host_link_state{
  uint8_t up : 1; //1 if there is a link to the host

  //For TX packet currently in flight
  uint8_t  tx_buf[LINK_MAX_ENCFRAME]; //The extra 2 is for cobs encoding overhead + zero byte
  uint8_t *tx_pos; //for tracking position in the tx buffer
  uint8_t  tx_len; //Length of data to send, nonzero if a tx is in progress
  
  //For reading in an RX packet
  uint8_t  rx_buf[LINK_MAX_ENCFRAME];
  uint8_t *rx_pos; //will be > rx_buf if a receive is in progress
  
  //Buffer of commands to-be-executed
  link_command cmd_buf[CMD_BUF_SIZE];
  uint8_t cmd_tail;
  uint8_t cmd_count; //cmd_buf works as a ring buffer queue
  
  const struct _link_handlers_t *user_handlers;
  timer_t status_timer;
} host_link_state;


// Initializes the USB stack and host_link structure
void setup_host_link(host_link_state *link, const struct _link_handlers_t *link_handlers);

//Call this periodically to handle link-related stuff and process commands
void tick_host_link(host_link_state *link);

//TX methods

//returns false if there's no space for the current command
uint8_t host_link_send(host_link_state *restrict link, const link_command *restrict cmd);
//Sends a debug packet with the given data buffer (up to LINK_MAX_FRAME), blocks until sent
void    host_link_send_debug(host_link_state *restrict cfg, void *restrict buf, uint8_t len);

/*
 * Pass an instance of this (in PROGMEM) to setup_host_link()
 *
 * Use the following initialization technique:
 *
 * static const link_handlers_t myhandlers PROGMEM = {
 *   .op0 = myfunc0,
 *   .op1 = myfunc1
 *   //etc...
 * };
 *
 */
typedef struct _link_handlers_t {
  link_command_f op0;
  link_command_f op1;
  link_command_f op2;
  link_command_f op3;
  link_command_f op4;
  link_command_f op5;
  link_command_f op6;
  link_command_f op7;
  link_command_f op8;
  link_command_f op9;
  link_command_f op10;
  link_command_f op11;
  link_command_f op12;
  link_command_f op13;
  link_command_f op14;
  link_command_f op15;
  link_command_f op16;
  link_command_f op17;
  link_command_f op18;
  link_command_f op19;
  link_command_f op20;
  link_command_f op21;
  link_command_f op22;
  link_command_f op23;
  link_command_f op24;
  link_command_f op25;
  link_command_f op26;
  link_command_f op27;
  link_command_f op28;
  link_command_f op29;
  link_command_f op30;
  link_command_f op31;
  link_command_f op32;
  link_command_f op33;
  link_command_f op34;
  link_command_f op35;
  link_command_f op36;
  link_command_f op37;
  link_command_f op38;
  link_command_f op39;
  link_command_f op40;
  link_command_f op41;
  link_command_f op42;
  link_command_f op43;
  link_command_f op44;
  link_command_f op45;
  link_command_f op46;
  link_command_f op47;
  link_command_f op48;
  link_command_f op49;
  link_command_f op50;
  link_command_f op51;
  link_command_f op52;
  link_command_f op53;
  link_command_f op54;
  link_command_f op55;
  link_command_f op56;
  link_command_f op57;
  link_command_f op58;
  link_command_f op59;
  link_command_f op60;
  link_command_f op61;
  link_command_f op62;
  link_command_f op63;
  link_command_f op64;
  link_command_f op65;
  link_command_f op66;
  link_command_f op67;
  link_command_f op68;
  link_command_f op69;
  link_command_f op70;
  link_command_f op71;
  link_command_f op72;
  link_command_f op73;
  link_command_f op74;
  link_command_f op75;
  link_command_f op76;
  link_command_f op77;
  link_command_f op78;
  link_command_f op79;
  link_command_f op80;
  link_command_f op81;
  link_command_f op82;
  link_command_f op83;
  link_command_f op84;
  link_command_f op85;
  link_command_f op86;
  link_command_f op87;
  link_command_f op88;
  link_command_f op89;
  link_command_f op90;
  link_command_f op91;
  link_command_f op92;
  link_command_f op93;
  link_command_f op94;
  link_command_f op95;
  link_command_f op96;
  link_command_f op97;
  link_command_f op98;
  link_command_f op99;
  link_command_f op100;
  link_command_f op101;
  link_command_f op102;
  link_command_f op103;
  link_command_f op104;
  link_command_f op105;
  link_command_f op106;
  link_command_f op107;
  link_command_f op108;
  link_command_f op109;
  link_command_f op110;
  link_command_f op111;
  link_command_f op112;
  link_command_f op113;
  link_command_f op114;
  link_command_f op115;
  link_command_f op116;
  link_command_f op117;
  link_command_f op118;
  link_command_f op119;
  link_command_f op120;
  link_command_f op121;
  link_command_f op122;
  link_command_f op123;
  link_command_f op124;
  link_command_f op125;
  link_command_f op126;
  link_command_f op127;
} link_handlers_t;

#endif