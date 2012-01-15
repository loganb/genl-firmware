#include "cobs.h"

uint8_t stuff_frame(uint8_t *stuffed, const uint8_t *src, uint8_t len) {
  uint8_t *cobs_point, *dst = stuffed, tmp;
  
  cobs_point = dst++;
  for(uint8_t i = 0; i < len; i++) {
    tmp = src[i];
    if(tmp == 0) { //Encode the distance to the zero char
      *cobs_point = dst - cobs_point;
      cobs_point  = dst++;
    } else if(dst - cobs_point == 255) { //254 bytes have been copied
      *cobs_point = 0xFF; //254-bytes NOT followed by a zero
      cobs_point = dst++;
    } else {
      *dst++ = tmp;
    }
  }
  //Fill in the last cobs point to put a null past the end-of-frame
  *cobs_point = dst - cobs_point + 1;
  *dst++ = 0; //null terminate
  return dst - stuffed;
}


uint8_t unstuff_frame(uint8_t *dst, uint8_t *stuffed) {
  return 0;
}


static inline void swap(uint8_t *a, uint8_t *b) { uint8_t tmp = *a; *a = *b; *b = tmp;  }

/*****
 * Writes bytes to the tail buffer for the frame. Must always be called with count > 0
 * Should be called with inc_byte = 1 when writing out the final byte
 *
 */
static inline uint8_t *write_tail(uint16_t count, uint8_t *const tail, uint8_t inc_byte) {
  uint8_t *ptr = tail;
  
  while(1) {
    if(count > 0xFE) {
      count += inc_byte;
      *ptr++ = 0xFF;
      count = count - 0xFE;
    } else {
      *ptr++ = (uint8_t)count;
      break;
    }
  }
  //The first and last get swapped because the tail is read in reverse order and all the center bytes are 0xFF
  swap(tail, ptr - 1);

  return ptr;
}

/**
 * Tail-end COBS encoding. To allow completely linear encoding, COBS bytes
 * are backwards-looking rather than forwards. If a 0xFF is encoded (because a run 
 * of more than 254 bytes w/o a 0), the last byte of the frame is the next count
 *
 * NOTE: Needs to be optimized to use more pointers, less counters
 *
 * return: total number of bytes in stuffed buffer
 */
uint8_t tecobs_stuff_frame(uint8_t *restrict stuffed, const uint8_t *restrict src, const uint8_t len) {
  const uint8_t *end = src + len;
  uint8_t       *stuff_ptr = stuffed, c;
  
  //Starts at 1 so that the next zero will be enocded as off the start of the frame
  //Also, this implementation only works for runs < 65k
  uint16_t count = 1;
  uint8_t tail[8], *tail_ptr = tail, *tmp; //This limits the total length to 7*254 bytes!
  
  while(src < end) {
    c = *src++;
    if(c == 0) {
      if(count < 0xFF) {
        *stuff_ptr++ = (uint8_t)count;
      } else {
        //0xFF means over 254 bytes, with additional length counters at then end of the frame
        *stuff_ptr++ = 0xFF;
        count -= 0xFE;
        tail_ptr = write_tail(count, tail_ptr, 0);
      }
      count = 1;
    } else {
      *stuff_ptr++ = c;
      count++;
    }
  }
  //Write out the tail of overflow bytes
  for(tmp = tail; tmp < tail_ptr; tmp++) {
    *stuffed++ = *tmp++;
    count++;
  }
  //Write out initial (last) count, it may require additional tail bytes to be written
  tmp = write_tail(count, stuff_ptr, 1);
  *tmp++ = 0;
  return tmp - stuffed;
}

/************
 * Unstuffs a tecobs frame. It is unstuffed in place, returning the new (shorter) length of the buffer. 
 * NOTE: currently does not handle the 0xFF bytes for very long runs w/o zeros, so don't decode frames
 * with more than 253 bytes between zeros
 *
 */
uint8_t tecobs_unstuff_frame(uint8_t *restrict buf, uint8_t len) {
  uint8_t c;
  int16_t pos = len;
  
  
  c = *(buf+len);
  len--;
  pos -= c;
  while(pos > 0) {
    c = buf[pos]; //Next count of bytes to move backwards
    buf[pos] = 0;
    pos -= c;
  }
  
  return len;
}
