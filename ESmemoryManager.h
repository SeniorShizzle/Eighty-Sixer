//
//  ESmemoryManager.h
//  Eighty-Sixer
//
//  Created by Esteban Valle on 4/22/15.
//  Copyright (c) 2015 Esteban Valle. All rights reserved.
//

#ifndef __Eighty_Sixer__ESmemoryManager__
#define __Eighty_Sixer__ESmemoryManager__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "main.h"

extern uint8_t *stackPointer;
extern uint8_t *framePointer;
extern uint8_t *heapPointer;
extern uint8_t *currentInstructionByte;

bool setupVirtualMemory(int);
bool storeInstructionByte(uint8_t);
bool instructionLoadComplete();
bool programWriteIsLocked();
void printStackPointers();
void printProgramCode();

uint8_t readNextInstructionByte();

uint8_t jumpToReadAtInternalAddress(int);
uint8_t jumpToReadAtExternalAddress(uint8_t*);

int translateToRelativeAddress(int*);
int* translateToPhysicalAddress(int);

bool setMemoryAtPhysicalAddress(int*, int);
int  fetchMemoryAtPhysicalAddress(int*);

bool hasNextInstruction();

bool offsetProgramCounter(int);

char *int2bin(unsigned, char*);

#endif /* defined(__Eighty_Sixer__ESmemoryManager__) */



