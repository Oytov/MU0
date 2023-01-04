# include <stdbool.h>

# define MAX_LABEL_NAME_SIZE 32

// Unfortunately i have to do this, i dont know how to get around this
// Including "parsingfuncs.h" causes errors
static bool caseIns_strcmp(char *str1, char *str2) {
    char c1, c2;
    int i;

    for (i = 0; str1[i] && str2[i]; i++) {
        // Convert chars to lowercase (oOOoOooOOOoo branchless programming)
        c1 = str1[i] + (str1[i] >= 'A' && str1[i] <= 'Z') * 32;
        c2 = str2[i] + (str2[i] >= 'A' && str2[i] <= 'Z') * 32;

        if (c1 != c2)
            return false;
    }
    
    if (!str1[i] ^ !str2[i])
        return false;

    return true;
}

typedef struct Label_ {
    char name[MAX_LABEL_NAME_SIZE];
    int lineNum;
} label;

typedef struct TmpLabel_ {
    char name[MAX_LABEL_NAME_SIZE];
    int lineNum;
    int dispLineNum;
} tmpLabel;

int nameInLabelArray(char *name, label *labelArray, size_t labelArrLen) {
    for (int i = 0; i < labelArrLen; i++) {
        if (labelArray[i].name[0] == 0)
            return -1;

        if (caseIns_strcmp(name, labelArray[i].name))
            return i;
    }

    return -1;
}