# include <stdbool.h>

# define ISCHAR_EMPTY(ch) (ch == ' ' || ch == '\t' || ch == '\v' || ch == '\r' || ch == '\f')

static const int powerOfTenLookup[] = {10e-1, 10e0, 10e1, 10e2, 10e3, 10e4, 10e5, 10e6};

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

// Starts with
bool stringStartsWith(char *string, char *subString) {
    for (int i = 0; subString[i]; i++) {
        if (subString[i] != string[i])
            return false;
    }
    return true;
}