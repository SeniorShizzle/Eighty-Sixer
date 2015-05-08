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

    if (verbose) printf("Offset %#x\n", value);

    regB += value;      // apply the offset to register B
    regB = translateToPhysicalAddress(*regB);

    if (!setMemoryAtPhysicalAddress(regB, *regA)) quit(ADDRESS_FAULT);          // sets the memory
}

void mrmovl(){
    if (verbose) {
        printf("Memory-Register Move Long\n");
    }

}

void arithmetic(uint8_t instruction){
    if (verbose) {
        printf("Arithmetic Operation: ");
    }

}

void jump(uint8_t instruction){
    if (verbose) {
        printf("Jump Operation: ");
    }

}

void cmov(uint8_t instruction){
    if (verbose) {
        printf("Conditional Move: ");
    }

}

void call(){
    if (verbose) {
        printf("Call: ");
    }

}

void ret(){
    if (verbose) {
        printf("RETURN\n");
    }
    
}

void pushl(){
    if (verbose) {
        printf("Push Long\n");
    }
    
}

void popl(){
    if (verbose) {
        printf("Pop Long\n");
    }
    
}


/**
 *  Begins execution of the stored program code at whatever the current program counter is
 *
 *  @return FALSE if an error occurred
 */
bool startCycle(){
    uint8_t instruction = readNextInstructionByte();
    if (verbose) printf("Running Instruction Code: %#02X\n", instruction);

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












