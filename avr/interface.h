#ifndef __interface_h
  #define __interface_h
  #include <stdint.h>
  #include <avr/io.h>
  #include "avrcommon.h"
  #include "shiftout.h"
  #include "nopDelay.h"
  
  
  
#define CSLOW()      SetPin(CS, 0) 
#define CSHIGH()     SetPin(CS, 1) 

#define WRLOW()      SetPin(WR, 0) 
#define WRHIGH()     SetPin(WR, 1) 

#define OELOW()      SetPin(OE, 0) 
#define OEHIGH()     SetPin(OE, 1) 

#define HVENABLE()   SetPin(LIM, 0) 
#define HVDISABLE()  SetPin(LIM, 1) 


#define setData(A)              PORTA = A
#define dataBusModeOutput()     DDRA  = 0xFF
#define dataBusModeInput()      DDRA  = 0x00

extern IOPin_t LIM ;
extern IOPin_t CS  ;
extern IOPin_t OE  ;
extern IOPin_t WR  ;
extern IOPin_t D   ;
extern IOPin_t LA  ;
extern IOPin_t CK  ;

#define IDLE()     CSHIGH(); WRHIGH(); RDHIGH()

//#define WRPULSE()  CSLOW(); WRLOW(); NOP(); WRHIGH(); CSHIGH()

#define RDSTART()  CSLOW(); OELOW(); NOP()
#define RDFINISH() OEHIGH(); CSHIGH()


  
//  uint8_t getData     ( ) ;
  void    setAddress  ( uint32_t address) ;
  uint8_t ChipRead    ( uint32_t address) ;
  void    ChipWrite   ( uint32_t address, char data );
  uint8_t getID       ( );
  void    CHIPInit() ;


  extern char * IDSTRINGS[];
  extern uint8_t ChipSize[] ;
#endif  
