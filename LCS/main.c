#include "LCS.h"

int main() {
    char x[MAXLEN] = {"ABCBDAB"};
    char y[MAXLEN] = {"BDCABA"};
    int  c[MAXLEN][MAXLEN];     //only use one table
    int m, n;

    m = strlen(x);
    n = strlen(y);

    LCSLength(x, y, m, n, c);
    PrintLCS(c, x, m, n);
    printf("\n");

    return 0;
}
