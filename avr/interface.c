// Memory interface routines
// this layer does all the snot to read and write to the chip

#include "interface.h"


//port definitions
#define DATAPORT PORTA
#define DATAPIN  PINA
#define DATADDR  DDRA


char *   IDSTRINGS[] = {"NO MOD", "EPROM 16k", "EPROM 32k 50ms", "ROM 64k", "ROM 24P AUTO", "F"       , "G"       , "EPROM 32k 10ms", "I", "J", "K", "L", "EEPROM 16k"  , "N", "O", "P"};
uint8_t  ChipSize[]  = {0       , 7          , 0x0F            , 0x1F     , 0x3F          , 0x7F      , 0xFF      , 0x0F            , 0  , 0  , 0  , 0  , 7             , 0  , 0  , 0 };     // K<<8 | 0xFF
uint16_t WriteTime[] = {1       , 500        , 1               , 1        , 1             , 1         , 1         , 1               , 1  , 1  , 1  , 1  , 1             , 1  , 1  , 1 }; // in 100us increments.

// might be a big optimization here making these a macro!

IOPin_t LIM = { 4, &PIND }; //  Vpp 5V limiter                        
IOPin_t CS =  { 5, &PIND }; // /CS /E                                  ChipCTL C
IOPin_t OE =  { 6, &PIND }; // /OE /G  /RD                             ChipCTL B
IOPin_t WR =  { 7, &PIND }; // /WR /PGM (High voltage capable signal)  ChipCTL A


// ls595 stuff
IOPin_t D  =  { 0, &PINB };
IOPin_t LA =  { 1, &PINB };
IOPin_t CK =  { 2, &PINB };




void CHIPInit() {

  DDRA  = 0x00; // data bus to inputs
  
  DDRB  = 0xFF; // 
  
  
  /*
    PC0  ID0
    PC1  ID1
    PC2  ID2
    PC3  ID3  
  */
  
  DDRC  = 0xFF; // 
  PORTC = 0x0F; // ID pullups on
  
  PORTD = 0xFF; // 
  DDRD  = 0xFF; // 
}



//uint8_t getData() {
//  return  PINA;
//}

void setAddress(uint32_t address) {
  ShiftOutMSB(D, CK, (address >> 16) & 0xFF );
  ShiftOutMSB(D, CK, (address >> 8 ) & 0xFF );
  ShiftOutMSB(D, CK, (address      ) & 0xFF );
  StrobePulse(LA);
}

#define getData()              PINA

//#define RDSTART()  CSLOW(); OELOW(); NOP()

uint8_t ChipRead(uint32_t address) {
  uint8_t t;

  setAddress(address);
  RDSTART();  
  Delay(10);   
  t = getData();
  NOP();  NOP();  
  RDFINISH();
  Delay(2);
  return t;
  
}


void WRPulse(void) {
  CSLOW(); 
  WRLOW(); 

  NOP(); //!!!???!!! write delay

  WRHIGH(); 
  CSHIGH();
}




void ChipWrite(uint32_t address, char data) {
  dataBusModeOutput();
  setData(data);                         // data bits
  setAddress(address);
  
  CSLOW();   WRLOW(); 

  NOP(); //!!!???!!! write delay

  WRHIGH(); CSHIGH();
  
  dataBusModeInput();
}





uint8_t getID() {
  return ((~PINC) & 0x0F);
}


/*
void waitReady(void) {  // this is a hack, 
  uint8_t p, s;
  
  p = read(0x00);
  s = ~p;
  while(s != p) {
    p = s;
    s = read(0x00);        
  }
 
}
*/



/*

void DataBusModeOutput() {
  DATADDR = 0xFF;
}

void DataBusModeInput() {
  DATAPORT = 0x00;   // no residual on the bus
  DATADDR = 0x00;
}

void setData(uint8_t d) {
  DATAPORT =  data;   
}
*/





