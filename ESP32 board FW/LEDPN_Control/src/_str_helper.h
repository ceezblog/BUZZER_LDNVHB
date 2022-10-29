#ifndef _M_ARDUINO
#define _M_ARDUINO
#include <Arduino.h>
#endif

#ifndef _M_UDP_HELPER
#define _M_UDP_HELPER
#include "bz_udp_helper.h"
#endif


// return first space until reaching null or max string length-1 (BZ_DATALENGTH-1)
uint8_t _findSpace(char* text, uint8_t start_idx)
{
  if (text[0] == 0) return 0;
  uint8_t i = 0;
  while (i<BZ_DATALENGTH-1) //i<13
  {
    if (text[i]== 0 || text[i] == 0x20) //space = 32 or 0x20
      return i;
    i++;
  }
  
  return i; //return from 0 to BZ_DATALENGTH-1, to be in the safe size
}
