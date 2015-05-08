//
//  ESmemoryManager.c
//  Eighty-Sixer
//
//  Created by Esteban Valle on 4/22/15.
//  Copyright (c) 2015 Esteban Valle. All rights reserved.
//

#include "ESmemoryManager.h"

uint8_t *sandboxCeiling;
uint8_t *sandboxFloor;
uint8_t *nextInstructionByte;
uint8_t *lastInstructionByte;

uint8_t *currentInstructionByte;

uint8_t *stackPointer;
uint8_t *framePointer;
uint8_t *heapPointer;

int requestedSize;

bool initialized = false;
bool isLocked = false;


/**
 *  Sets up the virtual memory space. Should be called only once.
 *
 *  @param bytes the size of the virtual memory space to be created
 *
 *  @return TRUE if the virtual memory was successful. FALSE on error.
 */
bool setupVirtualMemory(int bytes){
    if (initialized) return false;              // if the memory has already been initialized, fail the function

    if (verbose) {
        printf("\nInitializing virtual memory space with %d bytes.\n", bytes);
    }

    requestedSize = bytes;

    sandboxCeiling = calloc(bytes, 1);          // calloc allocated memory for an array of size 'bytes' of 1 byte each, setting to zero in the process

    if (!sandboxCeiling) {                      // check to make sure calloc returned an appropriate chunk of memory
        return false;                           // return false on error
    }

    sandboxFloor = sandboxCeiling - bytes;      // the floor of the sandbox is the malloc pointer minus the byte length
    heapPointer  = sandboxFloor;                // before adding the program code, the heap pointer is the floor of the memory space
    nextInstructionByte = sandboxFloor;         // the program code goes at the bottom of the sandbox
    lastInstructionByte = sandboxFloor;         // start it at the bottom

    stackPointer = sandboxCeiling;              // the stack starts at the top
    framePointer = sandboxCeiling;              // nothing on the stack, so frame = stack

    if (verbose) printStackPointers();

    initialized = true;
    return 1;
}


/**
 *  Stores the instruction byte at the next lowest unoccupied adress.
 *  Assumes the instruction byte is valid. DO NOT CALL NAIVELY; PARSE INPUT FIRST.
 *
 *  @param byte the byte (char) to store
 *
 *  @return FALSE if an error occured during the store operation.
 */
bool storeInstructionByte(uint8_t byte){
    if (heapPointer >= stackPointer || !initialized) {          // if the heap pointer is at the stack pointer, we've overflowed the stack
        printf("FATAL ERROR: Stack Overflow / Segmentation Fault");
        quit(PROGRAM_ERROR);
    }

    if (isLocked) {
        printf("FATAL ERROR: Segmentation Fault: Attempting to overwrite program data");
        quit(PROGRAM_ERROR);
    }

    if (verbose) {
        printf("%02X ", byte);
        //if (instructionBytes % 2 == 0) printf(" ");
    }

    *nextInstructionByte = byte;                    // store the byte at the location of 
    nextInstructionByte++;

    return true;
}


/**
 *  Finalizes the instruction loading process and readies the virtual memory for program execution.
 *
 *  @return FALSE if an error occurred.
 */
bool instructionLoadComplete(){
    if (heapPointer >= stackPointer || !initialized) {          // if the heap pointer is at the stack pointer, we've overflowed the stack
        printf("\nFATAL ERROR: Stack Overflow / Segmentation Fault\n");
        return false;
    }

    lastInstructionByte = nextInstructionByte - 1;              // this is the address of the very last instruction
    heapPointer = nextInstructionByte;                          // start the bottom now we're here
    currentInstructionByte = sandboxFloor;                      // start reading at the very first byte

    isLocked = true;                                            // locks the program from entering more codes


    if (verbose) {
        printf("\nInstruction loading complete.\n");
        printStackPointers();
        printProgramCode();
    }

    return true;
}


/**
 *  Prints the current state of stack pointers, as well as the size of each memory segment.
 */
void printStackPointers(){

    printf("\nHere's your memory space, captain.\n---------------------------------------\n");
    printf("Sandbox Floor:              %p\n", sandboxFloor);
    printf("Sandbox Ceiling:            %p\n", sandboxCeiling);
    printf("Next Instruction Pointer:   %p\n", nextInstructionByte);
    printf("Last Instruction Pointer:   %p\n", lastInstructionByte);
    printf("Current Instruction:        %p\n", currentInstructionByte);
    printf("Stack Pointer:              %p\n", stackPointer);
    printf("Frame Pointer:              %p\n", framePointer);
    printf("Heap Pointer:               %p\n", heapPointer);
    printf("Sandbox Size/Specified:     %ld/%d\n", sandboxCeiling - sandboxFloor, requestedSize);
    printf("Stack Size:                 %ld\n",  sandboxCeiling - stackPointer);
    printf("Current Stack Frame Size:   %ld\n",  framePointer - stackPointer);
    printf("Heap Size:                  %ld\n",  heapPointer - nextInstructionByte);
    printf("Program Code Size:          %ld\n",  nextInstructionByte - sandboxFloor);
    printf("Instruction Bytes Read:     %d\n", instructionBytes);


    if (sandboxCeiling - sandboxFloor == requestedSize && nextInstructionByte - sandboxFloor == instructionBytes) {
        printf("\nLooks like we're all set to go!\n");
    } else {
        printf("\nLooks like there's something wrong.\n");
    }

}


/**
 *  Prints the entirety of the stored program code. Will only execute is the program code write
 *  operation is locked. See programWriteIsLocked() for more information.
 */
void printProgramCode(){
    if (!isLocked) return;              // we can only print the program code once it has been fully entered

    printf("\nYour Program:\n");

    for (uint8_t *i = sandboxFloor; i <= lastInstructionByte; i++) {
        printf("%02X ", *i);
        //printf("%s ", int2bin( *i, NULL));
        //if (i % 2 == 1) printf(" ");
    }

    printf("\n\n");
}


/**
 *  Returns TRUE if the program code write operation is terminated.
 *  If TRUE, the program code cannot be overwritten, and attempts to do so will fail.
 *
 *  @return TRUE if the program code write cycle is locked
 */
bool programWriteIsLocked(){
    return isLocked;
}





/**
 *  Translates the given pointer into the internal address system.
 *
 *  @param address a pointer within the virtual memory
 *
 *  @return the adjusted address, treating the base of the memory space as 0x000000000
 */

int physicalToRelativeAddress(int* address){
    if (address > (int *)sandboxCeiling || address < (int *)sandboxFloor) {       // the pointer is not within the virtual memory space
        return -1;
    }


    return (int)(address - (int *)sandboxFloor);                                   // subtraction does the trick
}

/**
 *  Translates the given internal address into the external address system.
 *
 *  @param address an address within the virtual memory, treating the base of the memory space as 0x00000000
 *
 *  @return a pointer to the physical memory address referenced by the internal address
 */
int* relativeToPhysicalAddress(int address){
    if (address > requestedSize || address < 0) {       // the pointer is not within the virtual memory space
        return NULL;
    }


    return (int *)sandboxFloor + address;                      // addition does the trick here
}


/**
 *  Reads the next lowest unread instruction byte and increments the counter for iteration.
 *
 *  @return the instruction byte
 */
uint8_t readNextInstructionByte(){
    if (!isLocked || !initialized) {
        printf("\nFATAL ERROR: Concurrent Modification / Read Exception\n");
        quit(ADDRESS_FAULT);
    }

    if (currentInstructionByte >= stackPointer || currentInstructionByte > lastInstructionByte) {
        printf("\nFATAL ERROR: Stack Overflow / Segmentation Fault\n");
        quit(ADDRESS_FAULT);
    }

    return *(currentInstructionByte++);                     // post-increment will return the proper byte and then increment for future calls

}


/**
 *  Jumps to the internal memory address and returns the byte located there. Automatically increments the current instruction.
 *  Fails by halting execution if the jump address is not a valid program instruction address.
 *
 *  @param address the internal memory address target of the jump instruction, treating the bottom of the sandbox as 0x00000000
 *
 *  @return the byte located at that address
 */
uint8_t jumpToReadAtInternalAddress(int address){
    if (!isLocked || !initialized) {
        printf("\nFATAL ERROR: Concurrent Modification / Read Exception\n");
        quit(ADDRESS_FAULT);
    }

    currentInstructionByte = (uint8_t *)relativeToPhysicalAddress(address);


    if (currentInstructionByte >= stackPointer || currentInstructionByte > lastInstructionByte) {
        printf("\nFATAL ERROR: Stack Overflow / Segmentation Fault\n");
        quit(ADDRESS_FAULT);
    }

    return *(currentInstructionByte++);                     // post-increment will return the proper byte and then increment for future calls
}


/**
 *  Jumps to the physical memory address and returns the byte located there. Automatically increments the current instruction.
 *  Fails by halting execution if the jump address is not a valid program instruction address.
 *
 *  @param address the physical memory address target of the jump instruction
 *
 *  @return the byte located at that address
 */
uint8_t jumpToReadAtExternalAddress(uint8_t *address){
    if (!isLocked || !initialized) {
        printf("\nFATAL ERROR: Concurrent Modification / Read Exception\n");
        quit(ADDRESS_FAULT);
    }

    currentInstructionByte = address;


    if (currentInstructionByte >= stackPointer || currentInstructionByte > lastInstructionByte) {
        printf("\nFATAL ERROR: Stack Overflow / Segmentation Fault\n");
        quit(ADDRESS_FAULT);
    }

    return *(currentInstructionByte++);                     // post-increment will return the proper byte and then increment for future calls
}

/**
 *  Offsets the program counter by an integer of bytes. Halts program execution if the new address is unlawful.
 *
 *  @param offset the offset to apply to the program counter
 *
 *  @return FALSE if an error occured, otherwise TRUE
 */
bool offsetProgramCounter(int offset){
    currentInstructionByte += offset;

    if (currentInstructionByte >= stackPointer || currentInstructionByte > lastInstructionByte) {
        printf("\nFATAL ERROR: Stack Overflow / Segmentation Fault\n");
        quit(ADDRESS_FAULT);
    }

    return true;
}

/**
 *  Returns TRUE if the program counter has not reached the end of the program.
 *
 *  @return TRUE if there are more instructions to be read
 */
bool hasNextInstruction(){
    if (currentInstructionByte >= stackPointer || currentInstructionByte > lastInstructionByte) return false;


    return true;
}


bool setMemoryAtPhysicalAddress(int* address, int payload){
    if (address > (int *)heapPointer || address < (int *)lastInstructionByte) return false;               // make sure we're trying to write memory to the heap

    *address = payload;

    if (*address == payload) return true;

    return false;
}

int fetchMemoryAtPhysicalAddress(int* address){
    if (address > (int *)heapPointer || address < (int *)lastInstructionByte) return -1;               // make sure we're trying to read memory from the heap

    return *address;
}




