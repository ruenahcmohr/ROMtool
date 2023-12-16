
#include "util.h"

/*

void printAddress( unsigned long address) {
  USART_printstring("0x");
  USART_printhex((address>>24) & 0xFF);
  USART_printhex((address>>16) & 0xFF);
  USART_printhex((address>>8) & 0xFF);
  USART_printhex(address & 0xFF);
}
*/

uint8_t get8(void) {
  uint8_t v;
  uint8_t t;
  t = USART_Receive(); v  = NibToBin(t) << 4;
  t = USART_Receive(); v |= NibToBin(t);
  return v;  
}

uint16_t get16(void) {
 uint16_t v;  
  v =  get8() << 8;
  v |= get8();  
  return v;
}

