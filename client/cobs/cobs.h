
#include <stdint.h>

/*
 * Implementation of consistent overhead byte stuffing
 */

/******
 * Stuffs and null terminates src into stuffed; stuffed buffer should be
 * 100.5% the length of src (rounded up)
 *
 * returns: final length of stuffed (including null)
 */
uint8_t stuff_frame(uint8_t *stuffed, const uint8_t *src, uint8_t len);

/*******
 * Unstuffs a null-terminated string into dst
 *
 * returns: final length of dst field
 */
uint8_t unstuff_frame(uint8_t *dst, uint8_t *stuffed);

/********
 * Stuffs a frame as a variant of COBS. In this mode, the indicies of the zeros
 * are backwards-looking, and runs longer than 254 chars have the extra bytes encoded 
 * at the end of the entire frame
 *
 */
uint8_t tecobs_stuff_frame(uint8_t *restrict stuffed, const uint8_t *const restrict src, const uint8_t len);

/************
 * Unstuffs a tecobs frame. It is unstuffed in place, returning the new (shorter) length of the buffer. 
 * NOTE: currently does not handle the 0xFF bytes for very long runs w/o zeros, so don't decode frames
 * with more than 253 bytes between zeros
 *
 */
uint8_t tecobs_unstuff_frame(uint8_t *restrict buf, uint8_t len);