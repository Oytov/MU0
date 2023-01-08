// Including headers
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <stdbool.h>

# include "caseIns_strcmp.h"
# include "label.h"
# include "instruction.h"
# include "parsingfuncs.h"

#ifdef _WIN32
# include <windows.h>
#endif

/* https://gist.github.com/rafaelglikis/ee7275bf80956a5308af5accb4871135
 * Cross-platform sleep function for C
 * @param int milliseconds
 */
void sleep_ms(int milliseconds) {
    #ifdef _WIN32
        Sleep(milliseconds);
    #elif _POSIX_C_SOURCE >= 199309L
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;
        nanosleep(&ts, NULL);
    #else
        usleep(milliseconds * 1000);
    #endif
}

# define MAX_LABELS 512
# define MAX_JUMPS  1024
# define MAX_CODE_LINE 32768

# define CHECK_OUT_OF_BOUNDS_ADDR(memsize, address) (address >= memsize || address < 0)

instruc code[MAX_CODE_LINE];

int MEMSIZE = 256;

// Function declaration
void MU0interpret(int lines, int clockrate);

// Main function
int main(int argc, char **argv) {
    // COMMAND LINE ARGUMENTS

    // Help message
    if (argc == 1) {
        puts("Arguments enclosed in <> are required. Ones enclosed in [] are optional.");
        puts("Usage:\nMU0 <file-path> [-memsize=some-number] [-clock=some-number]");
        return 0;
    }
    // Too many arguments
    if (argc > 4) {
        puts("Too many arguments");
        return 1;
    }

    int fileArg = -1,
        clockArg = 0,
        memSizeArg = 256;

    // Parsing arguments
    if (argc == 2)
        fileArg = 1;
    
    else {
        for (int i = 1; i < argc; i++) {
            if (stringStartsWith(argv[i], "-clock"))
                sscanf(argv[i], "-clock=%d", &clockArg);

            else if (stringStartsWith(argv[i], "-memsize"))
                sscanf(argv[i], "-memsize=%d", &memSizeArg);

            else
                fileArg = i;
        }
    }

    // Checking values of arguments passed
    if (fileArg == -1) {
        puts("No input file");
        return 1;
    }
    if (memSizeArg < 2) {
        puts("Memory size cannot be less than 2");
        return 1;
    }
    if (clockArg < 0) {
        puts("Clock rate cannot be less than 1Hz");
        return 1;
    }
    if (clockArg > 1000) {
        puts("Clock rate can't be more than 1kHz");
        return 1;
    }

    // Display warning if accuracy of clockrate drops below 90%
    if (clockArg != 0) {
        int tempCRA = (int)(1000 / clockArg);
        float clockRateAccuracy = 100.0f * (float)tempCRA / (1000.0f / clockArg);

        if (clockRateAccuracy < 90.0f)
            printf("Warning: the accuracy of clockrate is only %.2f%%\n", clockRateAccuracy);
    }

    MEMSIZE = memSizeArg;

    // Starting to read from the file
    FILE *fptr = fopen(argv[fileArg], "r");

    // Unable to open file
    if (!fptr) {
        printf("Unable to open file '%s'\n", argv[1]);
        return 1;
    }

    char line[216], *temp;
    int lineNum = 0, dispLine = 0, temp1, tempi, labeln = 0, tmpLabeln = 0;
    bool tmperr;

    // The labels array
    tmpLabel *tmpLabels = malloc(MAX_JUMPS * sizeof(tmpLabel));
    label *labels = malloc(MAX_LABELS * sizeof(label));

    // Safe guards
    if (tmpLabels == NULL) {
        printf("Error allocating %zuB of memory for array tmpLabels\n", MAX_JUMPS * sizeof(tmpLabel));
        return 1;
    }
    if (labels == NULL) {
        printf("Error allocating %zuB of memory for array labels\n", MAX_JUMPS * sizeof(label));
        return 1;
    }

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
        if (line[0] == 0)
            continue;

        // Doing shit
        temp = strtok(line, " ");

        // Load accumulator
        if (caseIns_strcmp(temp, "LDA")) {
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
            if (CHECK_OUT_OF_BOUNDS_ADDR(MEMSIZE, temp1)) {
                printf("Error on line %d. Memory address is out of bounds %d not in range 0 to %d\n", dispLine, temp1, MEMSIZE-1);
                return 1;
            }

            code[lineNum].arg = temp1;
        }

        // Store accumulator
        else if (caseIns_strcmp(temp, "STO")) {
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
            if (CHECK_OUT_OF_BOUNDS_ADDR(MEMSIZE, temp1)) {
                printf("Error on line %d. Memory address is out of bounds %d not in range 0 to %d\n", dispLine, temp1, MEMSIZE-1);
                return 1;
            }

            code[lineNum].arg = temp1;
        }

        // Add
        else if (caseIns_strcmp(temp, "ADD")) {
            code[lineNum].ins = ADD;
            code[lineNum].data = 0;

            temp = strtok(0, " ");

            // No arg error
            if (temp == NULL) {
                printf("Error on line %d. ADD requires an argument\n", dispLine);
                return 1;
            }

            // add acc
            if (caseIns_strcmp(temp, "acc")) {
                code[lineNum].data |= 0b100;
                lineNum++;
                continue;
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

            // Out of bounds mem addr
            if (isArgAddr(&code[lineNum]) && CHECK_OUT_OF_BOUNDS_ADDR(MEMSIZE, temp1)) {
                printf("Error on line %d. Memory address is out of bounds %d not in range 0 to %d\n", dispLine, temp1, MEMSIZE-1);
                return 1;
            }

            code[lineNum].arg = temp1;
        }

        // Subtract
        else if (caseIns_strcmp(temp, "SUB")) {
            code[lineNum].ins = SUB;
            code[lineNum].data = 0;

            temp = strtok(0, " ");

            // No arg error
            if (temp == NULL) {
                printf("Error on line %d. SUB requires an argument\n", dispLine);
                return 1;
            }

            // sub acc
            if (caseIns_strcmp(temp, "acc")) {
                code[lineNum].data |= 0b100;
                lineNum++;
                continue;
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

            // Out of bounds mem addr
            if (isArgAddr(&code[lineNum]) && CHECK_OUT_OF_BOUNDS_ADDR(MEMSIZE, temp1)) {
                printf("Error on line %d. Memory address is out of bounds %d not in range 0 to %d\n", dispLine, temp1, MEMSIZE-1);
                return 1;
            }

            code[lineNum].arg = temp1;
        }

        // Jump.
        else if (caseIns_strcmp(temp, "JMP")) {
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
        else if (caseIns_strcmp(temp, "JGE")) {
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
        else if (caseIns_strcmp(temp, "JNE")) {
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
        else if (caseIns_strcmp(temp, "STP")) {
            code[lineNum].ins = STP;
        }

        // Put accumulator to stdout
        else if (caseIns_strcmp(temp, "PUT")) {
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
    free(labels);

    // Finally interpret
    MU0interpret(lineNum, clockArg);
    
    return 0;
}

// The function that interpretes
void MU0interpret(int lines, int clockrate) {
    int *memory = calloc(MEMSIZE, sizeof(int)),
        acc = 0,
        instructionPointer,
        pointer,
        sleepTime = 0;

    if (clockrate > 0) sleepTime = 1000 / clockrate;
    
    // Safeguard
    if (memory == NULL) {
        printf("Error allocating %zuB of memory for array memory\n", MEMSIZE * sizeof(int));
        return;
    }

    for (instructionPointer = 0; instructionPointer <= lines; instructionPointer++) {
        switch (code[instructionPointer].ins) {
            // Load acc
            case LDA:
                // Pointer 
                if (isArgPtr(&code[instructionPointer])) {
                    pointer = memory[code[instructionPointer].arg];
                    if (CHECK_OUT_OF_BOUNDS_ADDR(MEMSIZE, pointer)) {
                        printf("Runtime error\nPointer at address %d is out of bounds. Pointer pointing to %d. ", code[instructionPointer].arg, pointer);
                        printf("Memory addresses are from 0 to %d\n", MEMSIZE -1);

                        free(memory);
                        return;
                    }

                    acc = memory[pointer];
                    break;
                }

                acc = memory[code[instructionPointer].arg];
                break;
            
            // Store acc
            case STO:
                // Pointer 
                if (isArgPtr(&code[instructionPointer])) {
                    pointer = memory[code[instructionPointer].arg];
                    if (CHECK_OUT_OF_BOUNDS_ADDR(MEMSIZE, pointer)) {
                        printf("Runtime error\nPointer at address %d is out of bounds. Pointer pointing to %d. ", code[instructionPointer].arg, pointer);
                        printf("Memory addresses are from 0 to %d\n", MEMSIZE -1);

                        free(memory);
                        return;
                    }

                    memory[pointer] = acc;
                    break;
                }

                memory[code[instructionPointer].arg] = acc;
                break;
            
            // Add num at addr / num to acc
            case ADD:
                if (isArgAddr(&code[instructionPointer])) {
                    // Pointer
                    if (isArgPtr(&code[instructionPointer])) {
                        pointer = memory[code[instructionPointer].arg];
                        if (CHECK_OUT_OF_BOUNDS_ADDR(MEMSIZE, pointer)) {
                            printf("Runtime error\nPointer at address %d is out of bounds. Pointer pointing to %d. ", code[instructionPointer].arg, pointer);
                            printf("Memory addresses are from 0 to %d\n", MEMSIZE -1);

                            free(memory);
                            return;
                        }

                        acc += memory[pointer];
                        break;
                    }

                    acc += memory[code[instructionPointer].arg];
                }
                else if ( isArgAcc(&code[instructionPointer]) ) {
                    acc += acc;
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
                        pointer = memory[code[instructionPointer].arg];
                        if (CHECK_OUT_OF_BOUNDS_ADDR(MEMSIZE, pointer)) {
                            printf("Runtime error\nPointer at address %d is out of bounds. Pointer pointing to %d. ", code[instructionPointer].arg, pointer);
                            printf("Memory addresses are from 0 to %d\n", MEMSIZE -1);

                            free(memory);
                            return;
                        }

                        acc -= memory[pointer];
                        break;
                    }

                    acc -= memory[code[instructionPointer].arg];
                }
                else if ( isArgAcc(&code[instructionPointer]) ) {
                    acc = 0;
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
    
    if (clockrate) sleep_ms(sleepTime);
    }

    free(memory);
}