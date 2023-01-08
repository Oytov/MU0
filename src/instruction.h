# include <stdbool.h>

/* INSTRUCTIONS
LDA - Load accumulator
    LDA $addr

STO - Store accumulator
    STO $addr

ADD - Addition
    ADD number / $addr

SUB - Subtraction
    SUB number / $addr

JMP - Unconditional jump
    JMP label

JGE - Jump if accumulator >= 0
    JGE label

JNE - Jump if accumulator != 0
    JNE label

STP - Stop program
    STP

SPECIAL INSTRUCTION(S)
PUT - Puts accumulator to stdout
    PUT
*/

enum instructionTypes {LDA, STO, ADD, SUB, JMP, JGE, JNE, STP,      // Normal Instructions
    PUT};                                                           // Special Instruction(s)

typedef struct Instruction_ {
    char ins;           // Line 30
    char data;          // Related to the arg, bit 1 is on if arg is an addr, (bit 2 is on if arg is a pointer) <-- maybe
    int  arg;           // Data of the arg

} instruc;

// Returns true if the argument of an instruction is a mem addr
bool isArgAddr(instruc *instruction) {
    return instruction->data & 0b01;
}

// Returns true if the argument of an instruction is a pointer to a mem addr
bool isArgPtr(instruc *instruction) {
    return instruction->data & 0b10;
}

// Special thing for add and sub instructions
bool isArgAcc(instruc *instruction) {
    return instruction->data & 0b100;
}