#include <stdlib.h>

void error(Error err, unsigned addr){
  
  switch(err){
    
    case ERR_ILLEGAL:
      printf("ERROR: %s reached at address %x\n","ILLEGAL ERROR",addr);
      exit();
    case ERR_CONDITION:
      printf("ERROR: %s reached at address %x\n","CONDITION ERROR",addr);
      exit();
    case ERR_IMMEDIATE:
      printf("ERROR: %s reached at address %x\n","IMMEDIATE ERROR",addr);
      exit();
    case ERR_SEGTEXT:
      printf("ERROR: %s reached at address %x\n","TEXT SEGMENTATION ERROR",addr);
      exit();
    case ERR_SEGDATA:
      printf("ERROR: %s reached at address %x\n","DATA SEGMENTATION ERROR",addr);
      exit();
    case ERR_SEGSTACK:
      printf("ERROR: %s reached at address %x\n","STACK SEGMENTATION ERROR",addr);
      exit();
    default:
      printf("ERROR: %s reached at address %x\n","UNKNOWN ERROR",addr);
      exit();   
  }

}

void warning(Warning warn, unsigned addr){
  printf("WARNING: HALT reached at address %x\n",addr);  
}
