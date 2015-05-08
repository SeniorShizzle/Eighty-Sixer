//
//  ESalu.c
//  Eighty-Sixer
//
//  Created by Esteban Valle on 4/20/15.
//  Copyright (c) 2015 Esteban Valle. All rights reserved.
//

#include "ESalu.h"

/* REGISTER ENCODINGS
    %eax		0
	%ecx		1
	%edx		2
	%ebx		3
	%esp		4
	%ebp		5
	%esi		6
	%edi		7
 */

int  registerA = 0;
int  registerB = 0;
int  registerC = 0;
int  registerD = 0;


int  sourceIndexPointer = 0;
int  destinationIndexPointer = 0;

bool zeroFlag       = false;
bool signFlag       = false;
bool overflowFlag   = false;


/** ALU OPERATIONS **/

void halt(){
    if (verbose) {
        printf("HALTING\n");
    }

    quit(HALT);
}

void noop(){                // no operation. waiting
    if (verbose) {
        printf("NO OPERATION\n");
    }

}

void rrmovl(){              // register to register move long
    if (verbose) {
        printf("Register-Register Move Long\n");
    }

    uint8_t registers = readNextInstructionByte();
    int *regA = registerAtIndex((registers & 0xF0) >> 4);
    int *regB = registerAtIndex(registers & 0xF);

    *regB = *regA;          // move contents of register A into register B
}

void irmovl(){              // immediate to register move long
    if (verbose) {
        printf("Immediate-Register Move Long: ");
    }

    uint8_t registers = readNextInstructionByte();
    int *regB = registerAtIndex(registers & 0xF);

    int value = 0;
    for (int i = 0; i < 32; i += 8) {                       // reading value bytes little endian
        value |= ((int)readNextInstructionByte() << i);
    }

    if (verbose) printf("%#x\n", value);

    *regB = value;
}

void rmmovl(){
    if (verbose) {
        printf("Register-Memory Move Long: ");
    }

    uint8_t registers = readNextInstructionByte();

    int *regA = registerAtIndex((registers & 0xF0) >> 4);
    int *regB = registerAtIndex(registers & 0xF);

    int value = 0;
    for (int i = 0; i < 24; i += 8) {
        value |= ((int)readNextInstructionByte() << i);
    }

    if (verbose) printf("Offset %#X\n", value);

    *regB += value;      // apply the offset to register B
    regB = relativeToPhysicalAddress(*regB);

    if (!setMemoryAtPhysicalAddress(regB, *regA)) quit(ADDRESS_FAULT);          // sets the memory
}

void mrmovl(){
    if (verbose) {
        printf("Memory-Register Move Long\n");
    }

    uint8_t registers = readNextInstructionByte();

    int *regA = registerAtIndex((registers & 0xF0) >> 4);
    int *regB = registerAtIndex(registers & 0xF);

    int value = 0;
    for (int i = 0; i < 24; i += 8) {
        value |= ((int)readNextInstructionByte() << i);
    }

    if (verbose) printf("Offset %#x\n", value);

    *regB += value;      // apply the offset to register B
    regB = relativeToPhysicalAddress(*regB);

    *regA = fetchMemoryAtPhysicalAddress(regB);
}

void arithmetic(uint8_t instruction){
    if (verbose) {
        printf("Arithmetic Operation: ");
    }

    uint8_t registers = readNextInstructionByte();

    int *regA = registerAtIndex((registers & 0xF0) >> 4);
    int *regB = registerAtIndex(registers & 0xF);

    uint8_t functionCode = instruction & 0xF;
    int result;

    switch (functionCode) {
        case 0:
            if (verbose) printf("add\n");

            if (*regA > 0 && *regB > INT_MAX - *regA) {             // check for overflow
                overflowFlag = true;
            } else if (*regA < 0 && *regB < INT_MIN - *regA) {
                overflowFlag = true;
            } else overflowFlag = false;

            result = *regB + *regA;
            break;
        case 1:
            if (verbose) printf("subtract\n");

            if (*regA > 0 && *regB > INT_MAX - *regA) {             // check for overflow
                overflowFlag = true;
            } else if (*regA < 0 && *regB < INT_MIN - *regA) {
                overflowFlag = true;
            } else overflowFlag = false;

            result = *regB - *regA;
            break;
        case 2:
            if (verbose) printf("and\n");
            result = *regB & *regA;
            overflowFlag = false;               // overflow flag always unset for bitwise operations
            break;
        case 3:
            if (verbose) printf("xor\n");
            result = *regB ^ *regA;
            overflowFlag = false;               // overflow flag always unset for bitwise operations
            break;

        default:
            quit(INSTRUCTION_FAULT);
            break;
    }

    if (result == 0) zeroFlag = true;           // zero flag is set when the resultant operation is a zero
    else zeroFlag = false;

    signFlag = (result >> 31);                  // sign flag is set to the leftmost bit. No need to mask, since 0 will pad 0 and 1 is true anyway


    *regB = result;                             // store the result in register B
}

void jump(uint8_t instruction){

    int value = 0;
    for (int i = 0; i < 32; i += 8) {
        value |= ((int)readNextInstructionByte() << i);
    }

    if (verbose) {
        printf("Jump Operation: %#X\n", value);
    }

    uint8_t functionCode = instruction & 0xF;
    switch (functionCode) {
        case 0:     // always
                                                        jumpToReadAtInternalAddress(value);
            break;
        case 1:     // less than equal
            if ((signFlag ^ overflowFlag) | zeroFlag)   jumpToReadAtInternalAddress(value);
            break;
        case 2:     // less than
            if (signFlag ^ overflowFlag)                jumpToReadAtInternalAddress(value);
            break;
        case 3:     // equal
            if (zeroFlag)                               jumpToReadAtInternalAddress(value);
            break;
        case 4:     // not equal
            if (!zeroFlag)                              jumpToReadAtInternalAddress(value);
            break;
        case 5:     // greater than equal
            if ( ~(signFlag ^ overflowFlag))            jumpToReadAtInternalAddress(value);
            break;
        case 6:     // greater than
            if (~(signFlag ^ overflowFlag) & ~zeroFlag) jumpToReadAtInternalAddress(value);
            break;

        default:
            quit(INSTRUCTION_FAULT);
            break;
    }

}

void cmov(uint8_t instruction){
    if (verbose) {
        printf("Conditional Move:\n");
    }

    uint8_t registers = readNextInstructionByte();

    int *regA = registerAtIndex((registers & 0xF0) >> 4);
    int *regB = registerAtIndex(registers & 0xF);

    uint8_t functionCode = instruction & 0xF;
    switch (functionCode) {
        case 1:     // less than equal
            if ((signFlag ^ overflowFlag) | zeroFlag)   *regB = *regA;
            break;
        case 2:     // less than
            if (signFlag ^ overflowFlag)                *regB = *regA;
            break;
        case 3:     // equal
            if (zeroFlag)                               *regB = *regA;
            break;
        case 4:     // not equal
            if (!zeroFlag)                              *regB = *regA;
            break;
        case 5:     // greater than equal
            if ( ~(signFlag ^ overflowFlag))            *regB = *regA;
            break;
        case 6:     // greater than
            if (~(signFlag ^ overflowFlag) & ~zeroFlag) *regB = *regA;
            break;

        default:
            quit(INSTRUCTION_FAULT);
            break;
    }

}

void call(){
    int value = 0;
    for (int i = 0; i < 32; i += 8) {
        value |= ((int)readNextInstructionByte() << i);
    }

    if (verbose) {
        printf("Call: %#X", value);
    }

    pushToStack(physicalToRelativeAddress((int *)currentInstructionByte));
    jumpToReadAtInternalAddress(value);

}

void ret(){
    if (verbose) {
        printf("RETURN\n");
    }

    jumpToReadAtInternalAddress(popFromStack());
    
}

void pushl(){
    uint8_t registers = readNextInstructionByte();
    int *regA = registerAtIndex((registers & 0xF0) >> 4);
    if ((registers & 0xF) != 0xF) quit(INSTRUCTION_FAULT);        // make sure the second register is the null register 0xF

    if (verbose) {
        printf("Push Long: %d\n", *regA);
    }

    pushToStack(*regA);
    
}

void popl(){
    if (verbose) {
        printf("Pop Long\n");
    }

    uint8_t registers = readNextInstructionByte();
    int *regA = registerAtIndex((registers & 0xF0) >> 4);
    if ((registers & 0xF) != 0xF) quit(INSTRUCTION_FAULT);        // make sure the second register is the null register 0xF

    *regA = popFromStack();
}


/**
 *  Begins execution of the stored program code at whatever the current program counter is
 *
 *  @return FALSE if an error occurred
 */
bool startCycle(){
    uint8_t instruction = readNextInstructionByte();
    if (verbose) printf("Running Instruction Code: %#02X at address %#04X\n", instruction, physicalToRelativeAddress((int *)currentInstructionByte));

    switch ((instruction & 0xF0) >> 4) {                   // mask to the leftmost nibble (icode)
        case 0:
            halt();
            break;
        case 1:
            noop();
            break;
        case 2:
            if ((instruction & 0xF) == 0) {                // the code 2 performs cmove if the ifun (rightmost byte) is set
                rrmovl();
            }else{
                cmov(instruction);
            }
            break;
        case 3:
            irmovl();
            break;
        case 4:
            rmmovl();
            break;
        case 5:
            mrmovl();
            break;
        case 6:
            arithmetic(instruction);
            break;
        case 7:
            jump(instruction);
            break;
        case 8:
            call();
            break;
        case 9:
            ret();
            break;
        case 0xA:
            pushl();
            break;
        case 0xB:
            popl();
            break;

        default:
            quit(INSTRUCTION_FAULT);            // if the icode is not one of the listed ones, we're screwed
            break;
    }

    printf("\n");

    return true;
}


int *registerAtIndex(int index){
    switch (index) {
        case 0:
            return &registerA;
        case 1:
            return &registerC;
        case 2:
            return &registerD;
        case 3:
            return &registerB;
        case 4:
            return (int *)&stackPointer;
        case 5:
            return (int *)&framePointer;
        case 6:
            return &sourceIndexPointer;
        case 7:
            return &destinationIndexPointer;


        default:
            return NULL;
            break;
    }
}













