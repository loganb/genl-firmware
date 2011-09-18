#ifndef __SPICONFIG_H_
#define __SPICONFIG_H_

/*
 *
 */
extern const void *_spi_lock;
extern uint8_t _spi_busy;

/*
 * Sets up the SPI interface as a master, full speed
 * Setup on the negedge
 */

void setup_spi(void);

/*
 * Returns a true-ish value if SPI is used by the module of the specified lock_val
 * Returns true if SPI is possessed by the given lock_val
 */
static inline uint8_t acquire_spi(const void *lock_val) {
    if(!_spi_lock) {
        return !!(_spi_lock = lock_val);
    } else {
        return _spi_lock == lock_val;
    }
}

static inline uint8_t has_spi(const void *lock_val) {
    return _spi_lock == lock_val;
}

static inline void release_spi(void) {
    _spi_lock = 0;
}


//uint8_t write_spi_byte_nb(spi_buf_t *sb);

/*
 * Writes a buffer out the SPI interface, returns immediately
 */
void write_spi(uint8_t *buf, short len);

/*
 * True if the SPI interface is busy sending a buf
 */
static inline uint8_t spi_busy(void) {
    return _spi_busy;
}

//void reset_latch(void);

//void set_latch(void);

#endif