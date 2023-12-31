#ifndef _HICOMMS_H
#define _HICOMMS_H   

  #include "usart.h"
  #include <stdint.h>


  void printNumDec32     ( uint32_t bv ) ;
  void printNumDec16     ( uint16_t bv ) ;
  void printNumDec8      ( uint8_t bv ) ;
  
  void printNumHex8      ( uint8_t i  );
  void printNumHex16     ( uint16_t v ) ;
  void printNumHex32     ( uint32_t v ) ;
  
  void USART_printstring ( char *data);

#endif
