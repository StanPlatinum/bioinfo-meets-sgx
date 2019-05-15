#ifndef NW_H
#define NW_H

#include <string>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

extern int  nw(string, string, string&, string&, int &, bool);

extern int  nw_align(int **, char **, string, string, string&, string&, int);

extern void  dpm_init        (int **, char **, int, int, int);
extern int   max             (int, int, int, char *);

#endif