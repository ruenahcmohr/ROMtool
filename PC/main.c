/*
             CREATOR: 
                DATE:
         DESCRIPTION:
            FILENAME:    .c
             STARTED:
    OPERATING SYSTEM: Linux
1st VERSION FINISHED:



*/


#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

//global variables


int main(int argc, char** argv, char *arge[]) {
  // local variables
  
  FILE * ihex;
  FILE * port;
  char temp[1025];
  int retcode;
  int end;
  
  retcode = 0;
  
  printf(" This program transfers ihex files to ROMTool \n");
  printf(" Written by Rue Mohr 2023 \n");
  
  
  if (argc < 2) {                          // do we have input paramiters??   
      puts("Need input paramiters   serial port, ihex file.\n");
      return 0;    
  }
  
  if ((port = fopen(argv[1], "wt")) == NULL) {  //open text file 'param 1' w/ err chk 
      printf("Unable to open %s for output.\n", argv[1]);
      return -1;
  }
  
  if ((ihex = fopen(argv[2], "rt")) == NULL) {  //open text file 'param 1' w/ err chk 
      printf("Unable to open %s for input.\n", argv[2]);
      return -1;
  }  
  
  fprintf(port, "\r"); fflush(port);
  sleep(1);
  fprintf(port, "W\r");fflush(port);
  sleep(1);  
    
  end = 0;
  while (!end) {  
    if (fgets ( temp, 1024, ihex ) == NULL)  end = 1 ;
    //fputs(temp, port);
    printf("%s", temp);
    if (temp[strlen(temp)-1] == '\n') temp[strlen(temp)-1] = '\r';
    fprintf(port, "%s", temp); fflush(port);
    temp[0] = '\0';
    usleep(1500000);
  }
  
  fclose(ihex);
  fclose(port);
  
  
  return(retcode);
}


//subroutines
