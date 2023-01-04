// Including headers
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <stdbool.h>

# include "label.h"
# include "instruction.h"
# include "parsingfuncs.h"

# define MAX_LABELS 256
# define MAX_JUMPS  512
# define MAX_CODE_LINE 32768
# define MEMSIZE 256

instruc code[MAX_CODE_LINE];
label labels[MAX_LABELS];

void MU0interpret(int lines);

// Main function
int main(int argc, char **argv) {
    // COMMAND LINE ARGUMENTS

    // Help message
    if (argc == 1) {
        puts("Usage: MU0 <file-path>");
        return 0;
    }
    // Too many arguments
    if (argc > 2) {
        puts("Too many arguments");
        return 1;
    }

    FILE *fptr = fopen(argv[1], "r");

    // Unable to open file
    if (!fptr) {
        printf("Unable to open file '%s'\n", argv[1]);
        return 1;
    }

    char line[216], *temp;
    int lineNum = 0, dispLine = 0, temp1, tempi, labeln = 0, tmpLabeln = 0;
    bool tmperr;

    tmpLabel *tmpLabels = calloc(MAX_JUMPS, sizeof(tmpLabel));

    // Loop line by line through file
    while (!feof(fptr)) {
        if (ferror(fptr)) {
            puts("Error while reading file");
            return 1;
        }

        if (fgets(line, sizeof(line), fptr) == NULL) break;        // Reads one line, if statement is needed to squish a pretty dum bug
        stripLine(line);

        dispLine++;

        // Skip if empty line
        if (line[0] == 0) {
            // dispLine++;
            continue;
        }

        // Doing shit
        temp = strtok(line, " ");

        // Load accumulator
        if (caseInsensitive_strcmp(temp, "LDA")) {
            code[lineNum].ins = LDA;
            code[lineNum].data = 0;

            temp = strtok(0, " ");

            // No mem addr error
            if (temp == NULL) {
                printf("Error on line %d. LDA requires a memory address\n", dispLine);
                return 1;
            }

            // Not memory addr
            if (temp[0] != '$') {
                printf("Error on line %d. '%s' is an invalid address\n", dispLine, temp);
                return 1;
            }

            temp++;
            code[lineNum].data |= 0b01;

            // If addr is pointer
            if (temp[0] == '*') {
                temp++;
                code[lineNum].data |= 0b10;
            }

            temp1 = strtoi(temp, &tmperr);

            // Unable to convert string to int
            if (tmperr) {
                printf("Error on line %d. Memory address isn't a number\n", dispLine);
                return 1;
            }

            // Out of bounds mem addr
            if (temp1 >= MEMSIZE || temp1 < 0) {
                printf("Error on line %d. Memory address is out of bounds %d not in range 0 - %d\n", dispLine, temp1, MEMSIZE-1);
                return 1;
            }

            code[lineNum].arg = temp1;
        }

        // Store accumulator
        else if (caseInsensitive_strcmp(temp, "STO")) {
            code[lineNum].ins = STO;
            code[lineNum].data = 0;

            temp = strtok(0, " ");

            // No mem addr error
            if (temp == NULL) {
                printf("Error on line %d. STO requires a memory address\n", dispLine);
                return 1;
            }

            // Not memory addr
            if (temp[0] != '$') {
                printf("Error on line %d. '%s' is an invalid address\n", dispLine, temp);
                return 1;
            }

            temp++;
            code[lineNum].data |= 0b01;

            // If addr is pointer
            if (temp[0] == '*') {
                temp++;
                code[lineNum].data |= 0b10;
            }

            temp1 = strtoi(temp, &tmperr);

            // Unable to convert string to int
            if (tmperr) {
                printf("Error on line %d. Memory address isn't a number\n", dispLine);
                return 1;
            }

            // Out of bounds mem addr
            if (temp1 >= MEMSIZE || temp1 < 0) {
                printf("Error on line %d. Memory address is out of bounds %d not in range 0 - %d\n", dispLine, temp1, MEMSIZE-1);
                return 1;
            }

            code[lineNum].arg = temp1;
        }

        // Add
        else if (caseInsensitive_strcmp(temp, "ADD")) {
            code[lineNum].ins = ADD;
            code[lineNum].data = 0;

            temp = strtok(0, " ");

            // No arg error
            if (temp == NULL) {
                printf("Error on line %d. ADD requires an argument\n", dispLine);
                return 1;
            }

            // Argument is memory address
            if (temp[0] == '$') {
                code[lineNum].data |= 0b01;

                temp++;

                // Pointer
                if (temp[0] == '*') {
                    temp++;
                    code[lineNum].data |= 0b10;
                }

                temp1 = strtoi(temp, &tmperr);

                // Out of bounds mem addr
                if (temp1 >= MEMSIZE || temp1 < 0) {
                    printf("Error on line %d. Memory address is out of bounds %d not in range 0 - %d\n", dispLine, temp1, MEMSIZE-1);
                    return 1;
                }
            }
            else if (temp[0] == '*') {
                printf("Error on line %d. Pointer cannot exist without being a memory address\n", dispLine);
                return 1;
            }
            else {
                temp1 = strtoi(temp, &tmperr);
            }

            // Unable to convert string to int
            if (tmperr) {
                if (isArgAddr(&code[lineNum]))
                    printf("Error on line %d. Memory address isn't a number\n", dispLine);
                else
                    printf("Error on line %d. Integer literal isn't a number\n", dispLine);
                return 1;
            }

            code[lineNum].arg = temp1;
        }

        // Subtract
        else if (caseInsensitive_strcmp(temp, "SUB")) {
            code[lineNum].ins = SUB;
            code[lineNum].data = 0;

            temp = strtok(0, " ");

            // No arg error
            if (temp == NULL) {
                printf("Error on line %d. SUB requires an argument\n", dispLine);
                return 1;
            }

            // Argument is memory address
            if (temp[0] == '$') {
                code[lineNum].data = 0b01;

                temp++;

                // Pointer
                if (temp[0] == '*') {
                    temp++;
                    code[lineNum].data |= 0b10;
                }

                temp1 = strtoi(temp, &tmperr);

                // Out of bounds mem addr
                if (temp1 >= MEMSIZE || temp1 < 0) {
                    printf("Error on line %d. Memory address is out of bounds %d not in range 0 - %d\n", dispLine, temp1, MEMSIZE-1);
                    return 1;
                }
            }
            else if (temp[0] == '*') {
                printf("Error on line %d. Pointer cannot exist without being a memory address\n", dispLine);
                return 1;
            }
            else {
                temp1 = strtoi(temp, &tmperr);
            }

            // Unable to convert string to int
            if (tmperr) {
                if (isArgAddr(&code[lineNum]))
                    printf("Error on line %d. Memory address isn't a number\n", dispLine);
                else
                    printf("Error on line %d. Integer literal isn't a number\n", dispLine);
                return 1;
            }

            code[lineNum].arg = temp1;
        }

        // Jump.
        else if (caseInsensitive_strcmp(temp, "JMP")) {
            code[lineNum].ins = JMP;

            temp = strtok(0, " ");

            // No label error
            if (temp == NULL) {
                printf("Error on line %d. JMP requires a label to jump to\n", dispLine);
                return 1;
            }

            // Put data into tmpLabels
            strncpy(tmpLabels[tmpLabeln].name, temp, MAX_LABEL_NAME_SIZE);
            tmpLabels[tmpLabeln].lineNum = lineNum;
            tmpLabels[tmpLabeln].dispLineNum = dispLine;

            tmpLabeln++;
        }

        // Jump if accumulator >= 0
        else if (caseInsensitive_strcmp(temp, "JGE")) {
            code[lineNum].ins = JGE;

            temp = strtok(0, " ");

            // No label error
            if (temp == NULL) {
                printf("Error on line %d. JGE requires a label to jump to\n", dispLine);
                return 1;
            }

            // Put data into tmpLabels
            strncpy(tmpLabels[tmpLabeln].name, temp, MAX_LABEL_NAME_SIZE);
            tmpLabels[tmpLabeln].lineNum = lineNum;
            tmpLabels[tmpLabeln].dispLineNum = dispLine;

            tmpLabeln++;
        }

        // Jump if accumulator != 0
        else if (caseInsensitive_strcmp(temp, "JNE")) {
            code[lineNum].ins = JNE;

            temp = strtok(0, " ");

            // No label error
            if (temp == NULL) {
                printf("Error on line %d. JNE requires a label to jump to\n", dispLine);
                return 1;
            }

            // Put data into tmpLabels
            strncpy(tmpLabels[tmpLabeln].name, temp, MAX_LABEL_NAME_SIZE);
            tmpLabels[tmpLabeln].lineNum = lineNum;
            tmpLabels[tmpLabeln].dispLineNum = dispLine;

            tmpLabeln++;
        }

        // Stop program
        else if (caseInsensitive_strcmp(temp, "STP")) {
            code[lineNum].ins = STP;
        }

        // Put accumulator to stdout
        else if (caseInsensitive_strcmp(temp, "PUT")) {
            code[lineNum].ins = PUT;
        }

        // Labels and who knows what
        else {
            for (tempi = 0; temp[tempi]; tempi++); tempi--;
            
            // Label
            if (temp[tempi] == ':') {               // TODO: check label size
                temp[tempi] = 0;

                strncpy(labels[labeln].name, temp, MAX_LABEL_NAME_SIZE);
                labels[labeln].lineNum = lineNum;
                
                labeln++;
            }

            // The shit
            else {
                printf("Error on line %d. Unknown instruction '%s'\n", lineNum, temp);
                return 1;
            }

            // dispLine++;
            continue;
        }

        lineNum++;
    }

    fclose(fptr);

    // Loop through the temporary label array
    // If the label name exists in the labels array then update line number in the code array
    // else error
    int labelNo;
    for (int i = 0; i < tmpLabeln; i++) {
        labelNo = nameInLabelArray(tmpLabels[i].name, labels, MAX_JUMPS);
        // printf("labelNo: %d\n", labelNo);

        if (labelNo != -1) {
            code[tmpLabels[i].lineNum].arg = labels[labelNo].lineNum;
        }
        else {
            printf("Error on line %d. Unknown label '%s'\n", tmpLabels[i].dispLineNum, tmpLabels[i].name);
            free(tmpLabels);
            return 1;
        }
    }

    free(tmpLabels);

    // Finally interpret
    MU0interpret(lineNum);
    
    return 0;
}

// The function that interpretes
void MU0interpret(int lines) {
    int memory[MEMSIZE] = {0},
        acc = 0,
        instructionPointer;

    for (instructionPointer = 0; instructionPointer <= lines; instructionPointer++) {
        switch (code[instructionPointer].ins) {
            // Load acc
            case LDA:
                // Pointer 
                if (isArgPtr(&code[instructionPointer])) {
                    puts("No functionality for pointers");
                    break;
                }

                acc = memory[code[instructionPointer].arg];
                break;
            
            // Store acc
            case STO:
                // Pointer 
                if (isArgPtr(&code[instructionPointer])) {
                    puts("No functionality for pointers");
                    break;
                }

                memory[code[instructionPointer].arg] = acc;
                break;
            
            // Add num at addr / num to acc
            case ADD:
                if (isArgAddr(&code[instructionPointer])) {
                    // Pointer
                    if (isArgPtr(&code[instructionPointer])) {
                        puts("No functionality for pointers");
                        break;
                    }

                    acc += memory[code[instructionPointer].arg];
                }
                else {
                    acc += code[instructionPointer].arg;
                }
                break;
            
            // Subtract num at addr / num from acc
            case SUB:
                if (isArgAddr(&code[instructionPointer])) {
                    // Pointer
                    if (isArgPtr(&code[instructionPointer])) {
                        puts("No functionality for pointers");
                        break;
                    }

                    acc -= memory[code[instructionPointer].arg];
                }
                else {
                    acc -= code[instructionPointer].arg;
                }
                break;
            
            // Unconditional jump
            case JMP:
                instructionPointer = code[instructionPointer].arg -1;
                break;
            
            // Jump if acc >= 0
            case JGE:
                if (acc >= 0)
                    instructionPointer = code[instructionPointer].arg -1;
                break;
            
            // Jump if acc != 0
            case JNE:
                if (acc != 0)
                    instructionPointer = code[instructionPointer].arg -1;
                break;
            
            // Stop
            case STP:
                return;
                break;

            // SPECIAL

            // Put value at acc to stdout
            case PUT:
                printf("%d\n", acc);
                break;

            // Default
            default:
                printf("Oh no debugging time. Bytecode line: %d\n", instructionPointer);
                printf(".ins: %d\t.data: %d\t.arg%d\n", code[instructionPointer].ins, code[instructionPointer].data, code[instructionPointer].arg);
                break;
        }
    }
}

/*
TODO: pointer support
      add acc
      sub acc
*/