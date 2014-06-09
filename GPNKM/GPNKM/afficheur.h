#ifndef H_AFFICHEUR
#define H_AFFICHEUR
#include "structures.h"

int cmpfunct(TResults *a, TResults *b);
typedef int (*compfn)(const void*, const void*);
void showMainMenu();


#endif