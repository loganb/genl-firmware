

uint8_t stuff_frame(uint8_t *stuffed, const uint8_t *src, uint8_t len) {
  uint8_t *cobs_point, *dst = stuffed, tmp;
  
  cobs_point = dst++;
  for(uint8_t i = 0; i < len; i++) {
    tmp = src[i]
    //Do the cobs thing
    if(tmp == 0) {
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