#ifndef __ALLEGRO_H_
#define __ALLEGRO_H_

#include <stdint.h>
#include <avr/pgmspace.h>

typedef struct _allegro_spi_state {
    //Position of motor in units of 1/32 steps (0-127)
    //Position 0 is full current in A winding
    uint8_t motor_position;
    //Flag to indicate motor state needs to be updated
    uint8_t dirty : 1;
    //Flag to indicate motor needs to be initialized
    uint8_t do_init: 1;
    
    uint8_t current_level;
    
    //Transmit buffer
    uint8_t tx_buf[3];
} allegro_spi_state;

//Put an instance of this in PROGMEM
typedef struct PROGMEM _allegro_spi_cfg {
    //Pointer to the port with the strobe pin
    volatile uint8_t *strobe_port;
    //Bitmask that can be OR'd out the port to set strobe high
    uint8_t strobe_mask;
    
    //A register that can be written 0x00-0xFF to control the current from min-max respectively
    volatile uint8_t *pwm_register;

    //Pointer to the state struct
    allegro_spi_state *state;
} allegro_spi_cfg;

//Starts initialization of the motor
void init_motor(const allegro_spi_cfg *cfg);

//True if the motor is initialized and ready
uint8_t motor_ready(const allegro_spi_cfg *cfg);

//Should be called very regularly to update the state of the motor, always returns immediately
void tick_motor(const allegro_spi_cfg *cfg);

//Moves the motor forwards/backwards by <steps> number of 1/32nd microsteps
uint8_t move_motor(const allegro_spi_cfg *cfg, int8_t steps);

//Sets the motor to position <pos>, from 0-127, 1/32nd microsteps
uint8_t set_motor(const allegro_spi_cfg *cfg, uint8_t pos);

//Sets the motor current between min and max (0x00 - 0xFF)
void set_motor_current(const allegro_spi_cfg *cfg, uint8_t current);

#endif