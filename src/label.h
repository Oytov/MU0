# include <stdbool.h>

# define MAX_LABEL_NAME_SIZE 32

// Label struct, used for the labels
typedef struct Label_ {
    char name[MAX_LABEL_NAME_SIZE];
    int lineNum;
} label;

// Another Label struct, used for the jump instructions
typedef struct TmpLabel_ {
    char name[MAX_LABEL_NAME_SIZE];
    int lineNum;
    int dispLineNum;
} tmpLabel;

// Check if a label name is in a label struct array
int nameInLabelArray(char *name, label *labelArray, size_t labelArrLen) {
    for (int i = 0; i < labelArrLen; i++) {
        if (labelArray[i].name[0] == 0)
            return -1;

        if (caseIns_strcmp(name, labelArray[i].name))
            return i;
    }

    return -1;
}