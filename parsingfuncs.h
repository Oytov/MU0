# include <stdbool.h>

# define ISCHAR_EMPTY(ch) (ch == ' ' || ch == '\t' || ch == '\v' || ch == '\r' || ch == '\f')

static const int powerOfTenLookup[] = {1, 10, 100, 1000, 10000, 100000, 1000000};

// Strips all empty characters to the left of the string
void stripLine(char *string) {
    int starti, i;
    for (starti = 0; ISCHAR_EMPTY(string[starti]); starti++) continue;

    for (i = starti; string[i]; i++) {
        string[i - starti] = string[i];
    }

    string[i - starti] = 0;

    // Replaces the \n and ; with a \0 so it doesnt bother us
    for (i = 0; string[i]; i++) {
        string[i] = (string[i] == '\n' || string[i] == ';') ? 0 : string[i];
    }
}

// Is what it's name says it is
bool caseInsensitive_strcmp(char *str1, char *str2) {
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

// String to integer
int strtoi(char *string, bool *error) {
    int len = 0, out = 0;
    for (int i = 0; string[i]; i++) {
        if (string[i] < '0' || string[i] > '9') {
            *error = true;
            return 0;
        }
        len++;
    }

    if (len > 6) {
        *error = true;
        return 0;
    }

    for (int i = 0; i < len; i++) {
        out += (string[i] - '0') * powerOfTenLookup[len - 1 - i];
    }

    *error = false;
    return out;
}