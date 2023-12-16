
#ifndef __shiftcomm
  #define __shiftcomm
   
  #include <stdint.h> 
  #include "avrcommon.h"
  #include <avr/io.h>

  
  #define StrobePulse(P)          PulseHPin(P)  
  #define ShiftOutClockPulse(C)   SetPin(C, 1); NOP(); SetPin(C, 0)  
  #define ShiftOutSendOne(D,C)    SetPin(D, 1); ShiftOutClockPulse(C) 
  #define ShiftOutSendZero(D,C)   SetPin(D, 0); ShiftOutClockPulse(C)     
 
  void ShiftOutMSB (IOPin_t DataPin, IOPin_t ClkPin, uint8_t bits ) ;

#endif
