
#include "allegro.h"
#include "spi.h"

#include "util.h"

//Converts the position value (0-127) into an appropriate, 6-bit magnitude
inline static uint8_t motor_mag(uint8_t pos);
//Converts the position into a phase value of 0 for forward (positive), 1 for reverse (negative)
inline static uint8_t motor_phs(uint8_t pos);

void init_motor(const allegro_spi_cfg *cfg) {
    allegro_spi_state *state = (allegro_spi_state*)WORD_FIELD(cfg, state);
    volatile uint8_t *strobe = (volatile uint8_t*)WORD_FIELD(cfg, strobe_port);
    uint8_t strobe_mask = BYTE_FIELD(cfg, strobe_mask);
    
    state->motor_position = 0;
    state->dirty   = 1;
    state->do_init = 1;

    //THAR BE MAGIC HERE: DDRx register is 1 lower in mem than the PORTx register
    *(strobe - 1) |= strobe_mask; //Set the strobe pin as output
    *strobe |= strobe_mask; //Strobe is default high (not writing)
}

uint8_t motor_ready(const allegro_spi_cfg *cfg) {
  allegro_spi_state *state = (allegro_spi_state*)WORD_FIELD(cfg, state);

  return !state->do_init;
}

void tick_motor(const allegro_spi_cfg *cfg) {
    allegro_spi_state *state = (allegro_spi_state*)WORD_FIELD(cfg, state);
    volatile uint8_t *str = (uint8_t*)WORD_FIELD(cfg, strobe_port);
    uint8_t mask = BYTE_FIELD(cfg, strobe_mask);
    uint8_t pos, amag, bmag;

    if(state->do_init) {
        if(!has_spi(cfg)) {
            //Means we need to prep the motors
            if(!acquire_spi(cfg)) return; //Can't do it now
        
            //Pull strobe low
            *str &= ~mask;
        
            //Prep the buffer, constants to initialize the motor
            //Idle: 1 (not idle)
            //Osc control: 0 (4Mhz)
            //Fast Decay: 8 (17.75µs)
            //Off Time: 16 (33.75µs)
            //Blank Time: 6x (1.5µs)
            //Word select: 1
            state->tx_buf[0] = 0b00000100; //MSB D18-D16
            state->tx_buf[1] = 0b00000100; //D15-D8
            state->tx_buf[2] = 0b10000011; //LSB, D7-D0
        
            write_spi(state->tx_buf, 3);
        } else if(!spi_busy()){
            //Finish init process
            state->do_init = 0;
            //Restore high strobe to lock in values
            *str |= mask;
            //Release SPI
            release_spi();
        }
    } else if(state->dirty && !has_spi(cfg)) {
        if(!acquire_spi(cfg)) return; //Can't flush now

        pos = state->motor_position;
        amag = motor_mag(pos); //cos
        bmag = motor_mag(pos-32); //90 degrees behind, sin

        state->tx_buf[0] = 0b00000110; //B Mixed decay, external Vref, /8; MSB D18-D16
        state->tx_buf[1] = 0 << 7 |                 //A Mixed decay D15
                           (motor_phs(pos-32) << 6) | //Bphs D14
                           (motor_phs(pos)    << 5) | //Aphs D13
                           ((bmag >> 1) & 0x1F);       //BMag D12-D8; D15-D8
        state->tx_buf[2] = ((bmag & 0x01) << 7) | //BMag LSB (D7)
                           ((amag & 0x3F) << 1) | //AMag (D6 - D1)
                           0b00000000;           //WS: 0; LSB, D7-D0

        *str &= ~mask; //Set strobe low to begin filling controller register
        state->dirty = 0; //Clear dirty flag
        write_spi(state->tx_buf, 3); //Start SPI transfer
    } else if(has_spi(cfg)){
        //Means we are in the process of doing a transfer
        if(!spi_busy()) {
          *str |= mask; //Pull strobe high to signal end of transfer
          release_spi();
        }
    }
}

uint8_t move_motor(const allegro_spi_cfg *cfg, int8_t steps) {
  allegro_spi_state *state = (allegro_spi_state*)WORD_FIELD(cfg, state);
  
  state->motor_position = state->motor_position + steps;
  state->dirty = 1;
  return state->motor_position;
}

void set_motor_current(const allegro_spi_cfg *cfg, uint8_t current) {
  //allegro_spi_state *state = (allegro_spi_state*)WORD_FIELD(cfg, state);
  current_set_f func = (current_set_f)WORD_FIELD(cfg, current_control);
  
  //Vref can run from 0-2.6V, with voltage divider and PWM, Vref runs from 0-2.5,
  //TODO: A future revision of the board that yields full 8-bit precision over 0.5-2.6
  current = ((204 * current) / 256) + 51;
  func(current);
}



static const prog_uint8_t sin_table[] PROGMEM = {
    63,
    63,
    63,
    62,
    62,
    61,
    60,
    59,
    58,
    57,
    56,
    54,
    52,
    51,
    49,
    47,
    45,
    42,
    40,
    38,
    35,
    32,
    30,
    27,
    24,
    21,
    18,
    15,
    12,
    9,
    6,
    3,
    0
};

//
// Returns abs(cos(pos))
//   pos - ranges from 0-127 over entire circle
//   return - ranges from 0-63
//
//
inline static uint8_t motor_mag(uint8_t pos) {
  uint8_t quadrant = ((pos % 128) / 32) % 2; //Even or odd quadrants

  pos = pos % 32;

  if(quadrant) //quadrants 1 and 3
    return pgm_read_byte(sin_table + 32 - pos);
  else //quadrants 0 and 2
    return pgm_read_byte(sin_table + pos);
}

// Returns 1 if the cos(pos) is negative
//
inline static uint8_t motor_phs(uint8_t pos) {
  pos = (pos / 32) + 1; //pos == 0 or 1: +, pos == 2 or 3: -
  return (pos & 0x02) ? 1 : 0;
}