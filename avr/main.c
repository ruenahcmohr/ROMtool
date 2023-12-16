/*

main.c should be setup and console interface code.

1M chip is 1048576 bits ( 131072 bytes ) (last address 1FFFF)
2M chip is 2097152 bits ( 262144 bytes ) (last address 3FFFF)
4M chip is 4194304 bits ( 524288 bytes ) (last address 7FFFF)


todo:
  Make read accept start and end address
  Fix wait to check toggle lock and operation failure
   
   
  PA0 D0
  PA1 D1
  PA2 D2
  PA3 D3
  PA4 D4
  PA5 D5
  PA6 D6
  PA7 D7
  
  PB0 595 data
  PB1 595 latch
  PB2 595 clock
  PB3 
  PB4 
  PB5 
  PB6 
  PB7 
  
  PC0 
  PC1 
  PC2 
  PC3 
  PC4 
  PC5 
  PC6 
  PC7 
  
  PD0 RxD
  PD1 TxD
  PD2 
  PD3 
  PD4 VPP 5V limit. 
  PD5 /CS
  PD6 /RD
  PD7 /WR
   
   
  
*/

#include <avr/io.h>
#include "avrcommon.h"
#include "usart.h"
#include "hiComms2.h"
#include "nopDelay.h"
//#include "util.h"
//#include "interface.h"
#include "memops.h"
#include "interface.h"


uint8_t  BuffgetHex8    ( uint8_t DFVal) ;
uint16_t BuffgetHex16   ( uint16_t DFVal) ;
void     UIBuffSkipWhite( );
void     UIBuffNextWhite( ) ;
uint8_t  UIBuffGetChar  ( );
void     UIgetCommand   ( void) ;



//--------------------------------- UI Buffer stuff ----------------------------------------

#define ENTER '\r'


#define CMDBUFFSZ  32
char    cmdBuffer[CMDBUFFSZ];
uint8_t cmdCrs;
#define BuffCrsChar cmdBuffer[cmdCrs]


#define PushNibble(V,N) ((V)=((V)<<4)|((N)&0x0F))

uint8_t BuffgetHex4(uint8_t DFVal) {
  uint8_t c;
  
  if ((c = UIBuffGetChar()) == 0)    return DFVal; // end of input
  if (!CharIsHex(c)) {
    cmdCrs--;     // unconsume character, cause its               
    return DFVal; // not our food
  }
  return NibToBin(c);
}


uint8_t BuffgetHex8(uint8_t DFVal) {
  uint8_t v = 0;
  uint8_t t;
  
  t = BuffgetHex4(0x10);
  if (t == 0x10) return DFVal;    
  PushNibble(v,t);
  
  t = BuffgetHex4(0x10);
  if (t != 0x10)  PushNibble(v,t);
  
  return v;  
}

uint16_t BuffgetHex16(uint16_t DFVal) {
  uint16_t v = 0;
  uint8_t t;
  
  t = BuffgetHex4(0x10);
  if (t == 0x10) return DFVal;    
  PushNibble(v,t);
  
  t = BuffgetHex4(0x10);
  if (t != 0x10)  PushNibble(v,t); else return v;
  
  t = BuffgetHex4(0x10);
  if (t != 0x10)  PushNibble(v,t); else return v;
  
  t = BuffgetHex4(0x10);
  if (t != 0x10)  PushNibble(v,t);
  
  return v;  
}

void UIBuffSkipWhite() {
  while ((BuffCrsChar == ' ') && ( BuffCrsChar != ENTER )) cmdCrs++;
}

void UIBuffNextWhite() {
  while ((BuffCrsChar != ' ') && ( BuffCrsChar != ENTER )) cmdCrs++;
}

void UINextField() {
  UIBuffNextWhite(); UIBuffSkipWhite(); 
}

uint8_t UIBuffGetChar() {  
  uint8_t c;
  if ((c = BuffCrsChar) != ENTER) {
    cmdCrs++;
    return c;
  } else {
    return 0;      
  }
}



// fill buffer with blitherish from user.
void UIgetCommand(void) { 
  cmdCrs = 0;
 
  while(1) { //!!!???!!! we can code in for backspace later
  
    BuffCrsChar = USART_Receive();  
    
    if (0) {    
    } else if ((cmdCrs == (CMDBUFFSZ - 1)) && (BuffCrsChar != ENTER)) {
      USART_Transmit( '\a' );
    } else if (BuffCrsChar != ENTER) {
      USART_Transmit( BuffCrsChar );
      cmdCrs++;           
    } else {
      break;
    }
  }
  cmdCrs = 0;
  
  USART_printstring("\r\n");
  
}


//-------------------------------- Main loop interface ---------------------------------

void UIprintPrompt(void) {
  USART_printstring("\r\n>READY: ");
}


void cmdRead(void) {
  uint16_t start, end;
  
  UINextField();
  start = BuffgetHex16(0x0000); // start address
  UINextField();
  //  K<<8 | 0xFF
  end = ChipSize[getID()];
  end = (end<<8)|0xFF;
  end = BuffgetHex16(end); // end address   
  
  
  USART_printstring(">Reading chip from 0x");
  printNumHex16(start);
  USART_printstring(" to 0x");
  printNumHex16(end);
  USART_printstring("...\r\n");
     
  if (getID() == 1) { // 2716 make sure Vpp is 5V
    HVDISABLE();
    WRLOW();
  }   
     
  HexRead(start, end);   
}

void cmdErase(void) {
  USART_printstring(">I'm sorry Dave, I cannot do that.\r\n");
}

void cmdBlankCheck( ) {
  uint16_t  end;
  
  end = ChipSize[getID()];
  end = (end<<8)|0xFF;
  
  USART_printstring(">Checking chip...\r\n");
  ChipBlankCheck(end);
}

void cmdHexWrite() {
  HexWrite(getID());
}

void cmdGetID() {
  USART_printstring(">ID is: ");
  USART_printstring(  IDSTRINGS[getID()] );
  USART_printstring("\r\n");
}

void cmdHelp() {
  USART_printstring("> R   read  eprom\r\n");
  USART_printstring("> E   erase eprom\r\n");
  USART_printstring("> B   blank check\r\n");
  USART_printstring("> I   get chip ID\r\n");
  USART_printstring("> W   write ihex file\r\n");
}

//--------------------------------------- Main Loup ---------------------------------------

int main(void) {
  char command;

  USART_Init( 7 )   ; // 19200 at 14.7456 Mhz  when writing, wait for "OK." before sending next line!
 // USART_Init( 95 ); // 9600 at 14.7456 Mhz

  CHIPInit();
     
  Delay(4);  
  while(1) {
    UIprintPrompt();
    UIgetCommand();
    UIBuffSkipWhite();
    
    command = UIBuffGetChar();
    
    if (0){
    } else if (command == 'R') { // read      
      cmdRead();
    } else if (command == 'E') { // erase
      cmdErase();
    } else if (command == 'B') { // blank check
      cmdBlankCheck();
    } else if (command == 'W') { // write
      cmdHexWrite();
    } else if (command == 'I') { // get ID
      cmdGetID();
    } else if (command == '?') {
      cmdHelp();
    }  
  }
  
  return 0;
}





























