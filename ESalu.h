//
//  ESalu.h
//  Eighty-Sixer
//
//  Created by Esteban Valle on 4/20/15.
//  Copyright (c) 2015 Esteban Valle. All rights reserved.
//

#ifndef __Eighty_Sixer__ESalu__
#define __Eighty_Sixer__ESalu__

#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include "main.h"
#include <stdint.h>
#include "ESmemoryManager.h"

bool startCycle();

extern bool zeroFlag;
extern bool signFlag;
extern bool overflowFlag;

extern int stepCount;

void printHarmonFormattedTrace(char*);

int *registerAtIndex(int);

#endif /* defined(__Eighty_Sixer__ESalu__) */
