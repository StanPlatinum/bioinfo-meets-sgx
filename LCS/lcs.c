#include "LCS.h"

void LCSLength(char *x, char *y, int m, int n, int c[][MAXLEN]) {
    int i, j;

    for(i = 0; i <= m; i++)
        c[i][0] = 0;
    for(j = 1; j <= n; j++)
        c[0][j] = 0;
    for(i = 1; i<= m; i++) {
        for(j = 1; j <= n; j++) {
            if(x[i-1] == y[j-1]) {  
                c[i][j] = c[i-1][j-1] + 1;
            } else if(c[i-1][j] >= c[i][j-1]) {
                c[i][j] = c[i-1][j];
            } else {
                c[i][j] = c[i][j-1];

            }
        }
    }
}

void PrintLCS(int c[][MAXLEN], char *x, int i, int j) {
    if(i == 0 || j == 0)
        return;
    if(c[i][j] == c[i-1][j]) {
        PrintLCS(c, x, i-1, j);
    } else if(c[i][j] == c[i][j-1])
        PrintLCS(c, x, i, j-1);
    else {
        PrintLCS(c, x, i-1, j-1);
        printf("%c ",x[i-1]);
    }
}
