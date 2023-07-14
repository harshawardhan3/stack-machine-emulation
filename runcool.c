//  Author - Harshawardhan Mane

//  compile with:  cc -std=c11 -Wall -Werror -o runcool runcool.c

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//  THE STACK-BASED MACHINE HAS 2^16 (= 65,536) WORDS OF MAIN MEMORY
#define N_MAIN_MEMORY_WORDS (1<<16)

//  EACH WORD OF MEMORY CAN STORE A 16-bit UNSIGNED ADDRESS (0 to 65535)
#define AWORD               uint16_t
//  OR STORE A 16-bit SIGNED INTEGER (-32,768 to 32,767)
#define IWORD               int16_t

//  THE ARRAY OF 65,536 WORDS OF MAIN MEMORY
AWORD                       main_memory[N_MAIN_MEMORY_WORDS];

//  THE SMALL-BUT-FAST CACHE HAS 32 WORDS OF MEMORY
#define N_CACHE_WORDS       32

// a cache memory is intitialised wtih each of its word being obligatorily associated with an address and dirt status
struct CACHE_MEM {
    AWORD address;          //address
    IWORD contents;         //content-stored-in-the-cache
    bool clean;             //boolean-to-specify-the-state-of-the-location(dirty-or-clean)(default-is-dirty(set to 0) and 1 otherwise)
} cache_mem[N_CACHE_WORDS];

//  see:  https://teaching.csse.uwa.edu.au/units/CITS2002/projects/coolinstructions.php
enum INSTRUCTION {
    I_HALT       = 0,
    I_NOP,
    I_ADD,
    I_SUB,
    I_MULT,
    I_DIV,
    I_CALL,
    I_RETURN,
    I_JMP,
    I_JEQ,
    I_PRINTI,
    I_PRINTS,
    I_PUSHC,
    I_PUSHA,
    I_PUSHR,
    I_POPA,
    I_POPR
};

//  USE VALUES OF enum INSTRUCTION TO INDEX THE INSTRUCTION_name[] ARRAY
const char *INSTRUCTION_name[] = {
    "halt",
    "nop",
    "add",
    "sub",
    "mult",
    "div",
    "call",
    "return",
    "jmp",
    "jeq",
    "printi",
    "prints",
    "pushc",
    "pusha",
    "pushr",
    "popa",
    "popr"
};

//  ----  IT IS SAFE TO MODIFY ANYTHING BELOW THIS LINE  --------------


//  THE STATISTICS TO BE ACCUMULATED AND REPORTED
int n_main_memory_reads     = 0;
int n_main_memory_writes    = 0;
int n_cache_memory_hits     = 0;
int n_cache_memory_misses   = 0;

void report_statistics(void)
{
    printf("@number-of-main-memory-reads-(fast-jeq)\t%i\n",    n_main_memory_reads);
    printf("@number-of-main-memory-writes-(fast-jeq)\t%i\n",   n_main_memory_writes);
    printf("@number-of-cache-memory-hits\t%i\n",    n_cache_memory_hits);
    printf("@number-of-cache-memory-misses\t%i\n",  n_cache_memory_misses);
}

//  -------------------------------------------------------------------

//  EVEN THOUGH main_memory[] IS AN ARRAY OF WORDS, IT SHOULD NOT BE ACCESSED DIRECTLY.
//  INSTEAD, USE THESE FUNCTIONS read_memory() and write_memory()
//
//  THIS WILL MAKE THINGS EASIER WHEN WHEN EXTENDING THE CODE TO
//  SUPPORT CACHE MEMORY

//read-from-memory-function-implemented-with-write-back-cache-policy
AWORD read_memory(int address)
{
    int addr = address % N_CACHE_WORDS;
    if(cache_mem[addr].address == address) {
        ++n_cache_memory_hits;
        }
    else {
        ++n_cache_memory_misses;
        ++n_main_memory_reads;
        //write back the unit(byte-stored) to main memory if it is dirty
        if (cache_mem[addr].clean == false) {
            ++n_main_memory_writes;
            int addr_old = cache_mem[addr].address;
            main_memory[addr_old] = cache_mem[addr].contents;
        }
        //read-the-data-from-main-memory-and-update-the-addresses-of-the-cache-memory-and-add-the-data
        cache_mem[addr].address  = address;
        cache_mem[addr].contents = main_memory[address];
        //declare the unit(byte-stored) value to be clean
        cache_mem[addr].clean = true;
    }
    return cache_mem[addr].contents;
}
//write-to-memory-function-implemented-with-write-back-cache-policy
void write_memory(AWORD address, AWORD value)
{
    int addr = address % N_CACHE_WORDS;
    if(cache_mem[addr].address == address) {
        cache_mem[addr].contents = value;
    }
    else {
        //write-back-to-main-memory-if-the-unit(byte-stored)-is-dirty
        if(cache_mem[addr].clean == false) {
            int addr_old = cache_mem[addr].address;
            main_memory[addr_old] = cache_mem[addr].contents;
        }
        //updates-the-data-and-addresses-in-the-cache-memory
        cache_mem[addr].address  = address;
        cache_mem[addr].contents = value;
    }
    //declare the unit(byte-stored) value to be dirty again
    cache_mem[addr].clean = false;
}

//  -------------------------------------------------------------------

//  EXECUTE THE INSTRUCTIONS IN main_memory[]
int execute_stackmachine(void)
{
//  THE 3 ON-CPU CONTROL REGISTERS:
    int PC      = 0;                    // 1st instruction is at address=0
    int SP      = N_MAIN_MEMORY_WORDS;  // initialised to top-of-stack
    int FP      = 0;                    // frame pointer

//  REMOVE THE FOLLOWING LINE ONCE YOU ACTUALLY NEED TO USE FP

    while(true) {
    //variables to support our arithmetic functions
        IWORD val1;
        IWORD val2;

//  FETCH THE NEXT INSTRUCTION TO BE EXECUTED
        IWORD instruction   = read_memory(PC);
        ++PC;

//      printf("%s\n", INSTRUCTION_name[instruction]);

        if(instruction == I_HALT) {
            break;
        }

//  SUPPORT OTHER INSTRUCTIONS HERE
//      ....

            switch(instruction){
                case I_NOP:
                        //no_operation_is_being_performed
                            break;
                case I_ADD:
                        //top_two_values_from_the_stack_are_fetched
                            val1=read_memory(SP);++SP;
                            val2=read_memory(SP);
                        //the_two_values_are_added_and_pushed_to_the_top_of_the_stack
                            write_memory(SP, val1+val2);
                            break;
                case I_SUB:
                        //top_two_values_from_the_stack_are_fetched
                            val1=read_memory(SP);++SP;
                            val2=read_memory(SP);
                        //value_one_is_subtracted_from_value_two_and_pushed_to_the_top_of_the_stack
                            write_memory(SP, val2-val1);
                            break;
                case I_MULT:
                        //top_two_values_from_the_stack_are_fetched
                            val1=read_memory(SP);++SP;
                            val2=read_memory(SP);
                        //the_two_values_are_multiplied_and_pushed_to_the_top_of_the_stack
                            write_memory(SP, val1*val2);
                            break;
                case I_DIV:
                        //top_two_values_from_the_stack_are_fetched
                            val1=read_memory(SP);++SP;
                            val2=read_memory(SP);
                        //value_two_is_divided_by_value_one_and_pushed_to_the_top_of_the_stack
                            write_memory(SP, val2/val1);
                            break;
                case I_CALL:{
                        //the_address_of_the_next_instruction_to_be_performed_is_stored
                            IWORD addr=read_memory(PC);
                        //value_of_the_instruction_to_return_to_after_function_is_executed_pushed_to_the_stack
                            --SP;
                            write_memory(SP, PC+1);
                        //value_of_current_FP_is_pushed_onto_the_stack
                            --SP;
                            write_memory(SP, FP);
                        //PC-set-to-the-return-address-to-perform-normal-functioning-order
                            PC=addr;
                        //FP-set-to-the-current-SP
                            FP=SP;
                            break;
                            }
                case I_RETURN:{
                            //offset-to-be-added-to-FP-is-stored
                               IWORD offset=read_memory(PC);
                            //PC-is-set-to-it's-original-returning-value
                               PC=read_memory(FP+1);
                            //the-value-returned-by-the-function-is-pushed-on-TOS
                               int return_val=read_memory(SP);
                               write_memory(FP+offset, return_val);
                               int return_to =read_memory(FP);
                            //SP-set-to-FP-to-correct-it's-value-in-stack-frame
                               SP=FP+offset;
                            //FP-is-returned-to-it's-original-value
                               FP=return_to;
                               break;
                              }
                case I_JMP:{
                        //PC-is-set-to-the-address-the-function-is-meant-to-jump-to
                            int val=read_memory(PC);
                            PC=val;
                            break;}
                case I_JEQ:{
                        //PC-is-set-to-the-address-only-iff-the-value-on-the-TOS-is-zero
                            IWORD tos=read_memory(SP);++SP;
                            if(tos==0){int val=read_memory(PC);PC=val;}
                            else{++PC;}
                            break;
                           }
                case I_PRINTI:{
                        //print-the-integer-given-in-the-following-address
                               IWORD tos=read_memory(SP);
                               printf("%i", tos);++SP;
                               break;
                              }

                case I_PRINTS:{
                        //print-the-string-to-the-stdout-beginning-at-the-address-mentioned-following
                               int addr=read_memory(PC);
                               while(read_memory(addr)!=0){
                                    int word = read_memory(addr);
                                    AWORD chars[2]={word&0xff,word>>8};
                                    if(chars[0]=='\0'){break;}
                                    else if(chars[1]=='\0'){printf("%c",chars[0]);break;}
                                    else{printf("%c%c", chars[0],chars[1]);++addr;}
                               }
                               ++PC;
                               break;
                              }
                case I_PUSHC:
                        //push-the-constant-value-to-the-stack
                            val1=read_memory(PC);
                            ++PC;
                            --SP;
                            write_memory(SP, val1);
                            break;
                case I_PUSHA:{
                        //push-the-value-in-the-address-following-to-the-stack
                              int int_to_push_addr=read_memory(PC);
                              ++PC;
                              int int_to_push=read_memory(int_to_push_addr);
                              --SP;
                              write_memory(SP, int_to_push);
                              break;
                             }
                case I_PUSHR:{
                        //push-the-value-at-the-offset+FP-to-the-stack
                              IWORD offset=read_memory(PC);
                              ++PC;
                              int val=read_memory(FP+offset);
                              --SP;
                              write_memory(SP, val);
                              break;
                             }
                case I_POPA:{
                        //pop-the-value-at-the-TOS-and-write-it-to-the-address-following
                             IWORD pop_addr = read_memory(PC);
                             ++PC;
                             int tos=read_memory(SP);++SP;
                             write_memory(pop_addr, tos);
                             break;
                            }
                case I_POPR:{
                        //pop-the-value-mentioned-at-the-TOS-to-the-FP+offset-address-mentioned-following
                             IWORD offset=read_memory(PC);
                             ++PC;
                             int tos=read_memory(SP);++SP;
                             write_memory(FP+offset, tos);
                             break;
                            }
            }

    }

//  THE RESULT OF EXECUTING THE INSTRUCTIONS IS FOUND ON THE TOP-OF-STACK
    return read_memory(SP);
}

//  -------------------------------------------------------------------

//  READ THE PROVIDED coolexe FILE INTO main_memory[]
void read_coolexe_file(char filename[])
{
    memset(main_memory, 0, sizeof main_memory);   //  clear all memory

//  READ CONTENTS OF coolexe FILE
    FILE  *fp_in = fopen(filename, "rb");
    while(fread(main_memory, 2, N_MAIN_MEMORY_WORDS, fp_in)!=0){}
    fclose(fp_in);
}

void cache_initialisation(void)
{
    for(int addr=0;addr<N_CACHE_WORDS;++addr) {
        //cache-address-initialised-from-extreme-main-memory-location
        cache_mem[addr].address=N_MAIN_MEMORY_WORDS-1;
        cache_mem[addr].clean=false;
        }
}

//  -------------------------------------------------------------------

int main(int argc, char *argv[])
{
//  CHECK THE NUMBER OF ARGUMENTS
    if(argc != 2) {
        fprintf(stderr, "Usage: %s program.coolexe\n", argv[0]);
        exit(EXIT_FAILURE);
    }

//  READ THE PROVIDED coolexe FILE INTO THE EMULATED MEMORY
    read_coolexe_file(argv[1]);

//cache-memory's-initialisation
    cache_initialisation();

//  EXECUTE THE INSTRUCTIONS FOUND IN main_memory[]
    int result = execute_stackmachine();

    report_statistics();
//exit-the-program-with-printing-result-as-exit-status-value
    printf("@exit(%i)", result);

    return result;          // or  exit(result);
}
