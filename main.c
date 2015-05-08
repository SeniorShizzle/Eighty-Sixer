//
//  main.c
//  Eighty-Sixer™, The Little Y86 Virtual Machine
//  Reads little-endian machine code
//
//  Created by Esteban Valle on 4/20/15.
//  Copyright (c) 2015 Esteban Valle. All rights reserved.
//

#include "main.h"

#define ADDRESS_SPACE_SIZE_IN_BYTES 2048             // 2 MB is 2097152 bytes

void alpha();
void omega(FaultCode);

char *eighty_sixer =
" _____   _           _       _                     ____    _                                \n\
| ____| (_)   __ _  | |__   | |_   _   _          / ___|  (_) __  __   ___   _ __ TM         \n\
|  _|   | |  / _` | | '_ \\  | __| | | | |  _____  \\___ \\  | | \\ \\/ /  / _ \\ | '__|     \n\
| |___  | | | (_| | | | | | | |_  | |_| | |_____|  ___) | | |  >  <  |  __/ | |              \n\
|_____| |_|  \\__, | |_| |_|  \\__|  \\__, |         |____/  |_| /_/\\_\\  \\___| |_|        \n\
              |___/                 |___/                                                      ";


bool verbose  = false;
char *version = "0.5a";


int instructionBytes = 0;



/**
 *  The beginning. We're just getting started.
 */
void alpha(){
    // I am the alpha and the omega

    if (!setupVirtualMemory(ADDRESS_SPACE_SIZE_IN_BYTES)) {                                 // initialize the virtual memory space
        printf("\n\nFatal Error. Virtual address space could not be initialized.");
        omega(ADDRESS_FAULT);
    }

    if (verbose) {
        printf("\nReading instruction bytes from standard input:\n");
    }

    uint8_t byte = 0;
    char nibble = '0';
    while (nibble != '\0' && nibble != 'q') {
        scanf("%c", &nibble);
        byte = parseInput(nibble);

        if (byte < 0x10) {
            byte = (int)byte << 4;
            scanf("%c", &nibble);
            uint8_t nextByte = parseInput(nibble);

            if (nextByte < 0x10) {

                byte |= nextByte;
                instructionBytes++;

                storeInstructionByte(byte);
            }
        }
    }


    while (hasNextInstruction()) {                 // keep executing instructions until we've reached the end
        startCycle();
    }

    quit(HALT);

    //startCycle();   // lets get it started, it's HOT
}

/**
 *  Parses character input into hexidecimal uint8_t bytes. Only accepts proper hexidecimal input, null character, and Q/q for quit.
 *  Automatically increments the instruction bytes counter if and only if a valid byte is read.
 *
 *  @param byte the input byte to be parsed
 */
uint8_t parseInput(char byte){
    switch (byte) {
        case '0':
            return(0);
            break;
        case '1':
            return(1);
            break;
        case '2':
            return(2);
            break;
        case '3':
            return(3);
            break;
        case '4':
            return(4);
            break;
        case '5':
            return(5);
            break;
        case '6':
            return(6);
            break;
        case '7':
            return(7);
            break;
        case '8':
            return(8);
            break;
        case '9':
            return(9);
            break;
        case 'A':
        case 'a':
            return(10);
            break;
        case 'B':
        case 'b':
            return(11);
            break;
        case 'C':
        case 'c':
            return(12);
            break;
        case 'D':
        case 'd':
            return(13);
            break;
        case 'E':
        case 'e':
            return(14);
            break;
        case 'F':
        case 'f':
            return(15);
            break;
        case ' ':
            ///TODO: Fix this shit
            return 0xFF;
            break;

        case 'Q':
        case 'q':
            if (verbose) {
                printf("\n\n\nSigkill recieved. Goodbye.\n");
            }
            omega(INSTRUCTION_FAULT);
            break;
        case '\0':
            // we've reached the end of the file
            if (!instructionLoadComplete()){            // call instructionLoadComplete() to finish loading the instructions
                printf("\nFATAL ERROR. Exiting.\n");
                omega(INSTRUCTION_FAULT);
            }
            return 0xFF;
        default:
            return 0xFF;
            // it's not valid input!

    }


    /* Example Output
     Steps: 43
     PC: 0x00000108
     Status: HLT
     CZ: 0
     CS: 0
     CO: 0
     %eax: 0x0000001C
     %ecx: 0x00000000
     %edx: 0x00000000
     %ebx: 0xFFFFFFFD
     %esp: 0xFFFFFFDC
     %ebp: 0x00000100
     %esi: 0x00000000
     %edi: 0x0000A001
     
     */

    return -1;
}


/**
 *  This is the end. My only friend. The end.
 *
 *  @param faultCode the fault code conforming to the typdef FaultCode
 */
void omega(FaultCode faultCode){
    // the beginning and the end

    if (verbose) printStackPointers();

    switch (faultCode) {
        case HALT:
            break;
        case AOK:
            break;
        case ADDRESS_FAULT:
            break;
        case INSTRUCTION_FAULT:
            break;

        default:
            break;
    }


    // it's so hard to say goodbye.
    exit(0);
    // goodbye <3
}


/**
 *  Exits program operation by halting the ALU and printing the most recent ALU and memory state
 *
 *  @param faultCode the fault code conforming to the typedef FaultCode
 */
void quit(FaultCode faultCode){
    omega(faultCode);
}







/******** HERE BE DRAGONS ***********/


/**
 *  The primary function that is executed on program launch. Processes launch arguments.
 *
 *  @param argc the number of arguments passed
 *  @param argv a character pointer array containing string references of command arguments
 *
 *  @return 0 if program execution succeeds, otherwise undefined
 */
int main(int argc, const char * argv[]) {

    printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\");


    printf("\n\n                             Welcome to Esteban Valle's\n\n");
    printf("%s\n\n", eighty_sixer);
    printf("                     Y86 Virtual Machine. Live. Love. Eighty-Six™\n\n\n");
    printf("                         Come on then, feed me a couple bytes!\n\n");

    if (argc > 1) {     // argc is always 1, because argv[0] is the program's address when called
                        // this is the argument parser.
        printf("////////////////////////////////////////////////////////////////////////////////////////////////////\n\n");
        for (int i = 1; i < argc; i++){
            //printf("%s\n", argv[i]);

            if (!strncmp(argv[i], "-v", 2) || !strncmp(argv[i], "--verbose", 9)) {
                verbose = true;
                printf("\nVerbose mode, you sneaky dog you!\n");
            }

            if (!strncmp(argv[i], "-V", 2) || !strncmp(argv[i], "--version", 9)) {
                printf("Eighty-Sixer™ by Esteban Valle. Version %s\n", version);
                exit(0);
            }
            
        }
        
    }
    
    alpha();
    
    return 0;
}




/*
 Forms FORM-29827281-12:
 Test Assessment Report

 This was a triumph.
 I'm making a note here:
 HUGE SUCCESS.
 It's hard to overstate
 my satisfaction.
 Aperture Science
 We do what we must
 because we can.
 For the good of all of us.
 Except the ones who are dead.

 But there's no sense crying
 over every mistake.
 You just keep on trying
 till you run out of cake.
 And the Science gets done.
 And you make a neat gun.
 For the people who are
 still alive.

 Forms FORM-55551-5:
 Personnel File Addendum:

 Dear <<Subject Name Here>>,

 I'm not even angry.
 I'm being so sincere right now.
 Even though you broke my heart.
 And killed me.
 And tore me to pieces.
 And threw every piece into a fire.
 As they burned it hurt because
 I was so happy for you!
 Now these points of data
 make a beautiful line.
 And we're out of beta.
 We're releasing on time.
 So I'm GLaD. I got burned.
 Think of all the things we learned
 for the people who are
 still alive.

 Forms FORM-55551-6:
 Personnel File Addendum Addendum:

 One last thing:

 Go ahead and leave me.
 I think I prefer to stay inside.
 Maybe you'll find someone else
 to help you.
 Maybe Black Mesa...
 THAT WAS A JOKE. HA HA. FAT CHANCE.
 Anyway, this cake is great.
 It's so delicious and moist.
 Look at me still talking
 when there's Science to do.
 When I look out there,
 it makes me GLaD I'm not you.
 I've experiments to run.
 There is research to be done.
 On the people who are
 still alive.

 PS: And believe me I am
 still alive.
 PPS: I'm doing Science and I'm
 still alive.
 PPPS: I feel FANTASTIC and I'm
 still alive.

 FINAL THOUGHT:
 While you're dying I'll be
 still alive.

 FINAL THOUGHT PS:
 And when you're dead I will be
 still alive.
 
 STILL ALIVE
 
 Still alive.

                  =+$HM####@H%;,
              /H##############M$,
              ,@################+
               .H##############+
                 X############/
                  $##########/
                   %########/
                    /X/;;+X/

                     -XHHX-
                    ,######,
    #############X  .M####M.  X#############
    ##############-   -//-   -##############
    X##############%,      ,+##############X
    -##############X        X##############- 
     %############%          %############%
      %##########;            ;##########%
       ;#######M=              =M#######;
        .+M###@,                ,@###M+.
           :XH.                  .HX:
*/