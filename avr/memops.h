#ifndef __memops_h
#define __memops_h

  #include <stdint.h>
  #include "interface.h"
  #include "hiComms2.h"
  #include "util.h"

 void    HexRead       ( uint16_t start, uint16_t end); 
 void    HexWrite      ( uint8_t chipID);                   
 void    ChipBlankCheck( uint32_t address );                     
 uint8_t writeEEPROM28x( uint32_t address, uint8_t *buffer, uint8_t bytes)  ;
 uint8_t writeEPROM2716( uint32_t address, uint8_t *buffer, uint8_t bytes) ;
 uint8_t writeEPROM2732( uint32_t address, uint8_t *buffer, uint8_t bytes) ;

#endif
