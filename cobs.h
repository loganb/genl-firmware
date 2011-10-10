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