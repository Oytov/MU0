// Is what it's name says it is
bool caseIns_strcmp(char *str1, char *str2) {
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