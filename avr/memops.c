
/*

  This layer doesn't care how the interface works to the memory
  
*/

#include "memops.h"








void HexRead(uint16_t start, uint16_t end) {
  uint8_t checksum;
  uint32_t address;
  uint16_t i;
  uint8_t temp;
  
  address = start;
  
  
  while(address <= end) {

    i = Min( 0x10,  (end - address + 1));            
    
    USART_Transmit( ':' );
    printNumHex8(i);                    checksum = i; // byte count = 16
    
    printNumHex8((address>>8) & 0xFF);  checksum += ((address>>8) & 0xFF); // address H
    printNumHex8(address & 0xFF);       checksum += (address & 0xFF);      // address L
    
    USART_printstring("00");  // type 00 record (data)

    for( ; i ; address++, i--) {
     temp = ChipRead(address);            
     printNumHex8(temp);                checksum += temp;
    }
    
    printNumHex8(0-checksum);
    USART_printstring("\r\n");   
    
  }
  
  USART_printstring(":00000001FF\r\n"); // EOF

}







void ChipBlankCheck( uint32_t stop ) {

  uint32_t address;  
  uint8_t data;
              
  for( address = 0; address < stop; address++) {                      
    if ((data = ChipRead(address)) != 0xFF) {
       USART_printstring(">FAIL @ "); 
       printNumHex16(address); 
       USART_printstring(" ->"); 
       printNumHex8(data);   
       USART_printstring("\r\n");  
       return;
    }
  }
  USART_printstring("BLANK.\r\n");     
  
}


uint8_t writeEEPROM28x(uint32_t address, uint8_t *buffer, uint8_t bytes) {

  uint8_t errorCount;
  uint8_t i;
  uint8_t addCheck;
  uint8_t wd;


  addCheck = (address & 0xFF) & 0xF0;
  for(i = 0; i < bytes; i++) {           
        
    // write page, then pause for check on last byte, when ok, check all bytes.    
                                                                                                                                                  
    ChipWrite(address+i, buffer[i]);                    

    if ((((address+i+1)&0xFF) & 0xF0) != addCheck) {
      addCheck = ((address+i+1) & 0xFF) & 0xF0;

      for (wd = 0; wd < 128; wd++){           // write delay and check
        Delay(1000) ;
        if ( ChipRead(address+i) == buffer[i] ) break;
      }       
    }
  }
  if ((((address+i-1)&0xFF) & 0xF0) == addCheck) { // do any final writes.
    for (wd = 0; wd < 128; wd++){                  // write delay and check
      Delay(1000) ;
      if ( ChipRead(address+i) == buffer[i] ) break;
    }
  }
  // write finished, verify bytes
    
  errorCount = 0;
  for(i = 0; i < bytes; i++) {
    if ( buffer[i] != ChipRead(address+i)) {   
      USART_printstring("Write failure address:");  printNumHex16(address+i);  USART_printstring(".\r\n");	 
      errorCount++; // used as error flag
    }
  }

  return errorCount;
}



// The 2716 has backwards signals from everything else.
uint8_t writeEPROM2716(uint32_t address, uint8_t *buffer, uint8_t bytes) {

  uint8_t errorCount;
  uint8_t i;

  
 // for the 2716:
 // CS is P/E
 // OE is /G
 
 // CS   ChipCTL C
 // OE   ChipCTL B
 // WR   ChipCTL A (high voltage)
 
 // VPP     via ChipCTLA (WR) ** inverted
 // Program via ChipCTLC (CS)
 // /G      via ChipCTLB (OE)
 
  CSLOW();     // setup idle state
  OELOW();
  HVENABLE();  // let go of 5V limiter
  WRLOW();     // turn on HV 
 
  OEHIGH();
  dataBusModeOutput();
  for(i = 0; i < bytes; i++) {       
    setData(buffer[i]);                         // data bits
    setAddress(address+i);
    NOP(); NOP();
    CSHIGH();
    Delay(67000) ; //50ms!!!
    CSLOW();                      
  }   
  dataBusModeInput(); 
  OELOW();
  HVDISABLE();  // enable 5V limiter , we dont have to drive it low.
  
  // write finished, verify bytes
  errorCount = 0;
  for(i = 0; i < bytes; i++) {
    if ( buffer[i] != ChipRead(address+i)) {   
      USART_printstring("Write failure address:");  printNumHex16(address+i);  USART_printstring(".\r\n");	 
      errorCount++; // used as error flag
    }
  } 

  return errorCount;
}


// The 2732
uint8_t writeEPROM2732(uint32_t address, uint8_t *buffer, uint8_t bytes) {

  uint8_t errorCount;
  uint8_t i;
 
 // CS   ChipCTL C                [ pulse low 50ms]
 // WR   ChipCTL A (high voltage) [hold]
  
  
  CSHIGH();     // setup idle state
  HVENABLE();  // let go of 5V limiter
  WRLOW();     // turn on HV 
 
  dataBusModeOutput();
  for(i = 0; i < bytes; i++) {       
    setData(buffer[i]);                         // data bits
    setAddress(address+i);
    NOP(); NOP();
    CSLOW();
    // Delay(67000) ; // 50ms !!!
    Delay(13400) ; // 10ms
    
    CSHIGH();                      
  }   
  dataBusModeInput(); 
  WRHIGH();  // logic low pin 18
  HVDISABLE();  // enable 5V limiter , we dont have to drive it low.
  
  
  // write finished, verify bytes
  errorCount = 0;
  for(i = 0; i < bytes; i++) {
    if ( buffer[i] != ChipRead(address+i)) {   
      USART_printstring("Write failure address:");  printNumHex16(address+i);  USART_printstring(".\r\n");	 
      errorCount++; // used as error flag
    }
  } 

  return errorCount;
}


void HexWrite(uint8_t chipID) {
  uint8_t buffer[256];
  uint8_t checksum;
  uint8_t bytes, i;
  uint16_t  address;
  uint16_t  segment;
  uint8_t type;
  uint16_t end;
  uint32_t bigAddr;
    
  end = ChipSize[chipID];
  end = (end<<8)|0xFF;
  
  segment = 0;
  USART_printstring("READY FOR DATA:\r\n");
  while(1) {
    // Get the record.
    while( USART_Receive() != ':'); // wait for start
    bytes   = get8();            checksum =  bytes; 
    address = get16();           checksum += (address & 0xFF) + ((address>>8) & 0xFF);
    type    = get8();            checksum += type;
    
    for(i = 0; i < bytes; i++) {
      buffer[i] = get8();        checksum += buffer[i];
    }    
    checksum += get8();
    
    if (checksum != 0) {
      USART_printstring("SUM ERROR!\r\n");
      return;
    }

    if (0) {
    } else if (type == 1) {  // EOF
      USART_printstring("DONE.\r\n");
      return;
    } else if (type == 0) {  // data field
    
      bigAddr = segment; bigAddr <<= 16;   bigAddr += address;
         
                                                                 //!!!???!!! check write it not beyond end of memory    
      if (0) {
      } else if (chipID == 1) {  // 2716                                                           
        checksum = writeEPROM2716(bigAddr, buffer,  bytes);
      } else if (chipID == 2) {  // 2732
        checksum = writeEPROM2732(bigAddr, buffer,  bytes);
      } else if (chipID == 12) { // 2816      
        checksum = writeEEPROM28x(bigAddr, buffer,  bytes);
      } else {
        USART_printstring("NO AVAILABLE PROGRAMMING ALG.\r\n");
      }
      
      
      if (checksum == 0) { 
        USART_printstring("VERIFIED.\r\n");
      } 
      
   } else if (type == 2) { // extended address field
     segment = (buffer[1] | (buffer[0] << 8)) << 4;
     USART_printstring("OK.\r\n");
   }
 }

}

