//
//  main.h
//  Eighty-Sixer
//
//  Created by Esteban Valle on 5/7/15.
//  Copyright (c) 2015 Esteban Valle. All rights reserved.
//

#ifndef Eighty_Sixer_main_h
#define Eighty_Sixer_main_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ESalu.h"
#include "ESmemoryManager.h"
#include <stdint.h>


typedef enum FaultCode {
    HALT, AOK, ADDRESS_FAULT, INSTRUCTION_FAULT, PROGRAM_ERROR

} FaultCode;


extern bool verbose;
extern char *version;
extern int instructionBytes;

void quit(FaultCode);

uint8_t parseInput(char);



#endif
