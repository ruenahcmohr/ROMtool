#include "shiftout.h"


void ShiftOutMSB (IOPin_t DataPin, IOPin_t ClkPin, uint8_t bits ) {
  uint8_t mask;  
  for( mask = 1<<7; mask != 0; mask = mask >> 1) {    
    if ( (bits & mask) == 0 ) {
      ShiftOutSendZero(DataPin, ClkPin);
    } else {
      ShiftOutSendOne(DataPin, ClkPin);
    }    
  } 
}
