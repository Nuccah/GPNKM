#ifndef H_SERVEUR
#define H_SERVEUR
#include "structures.h"

int randomWeather();
double lapTime(TSect *tabSect);
double globalAverageSpeed(TCar pilot);
double lapAverageSpeed(TLap lap);
bool isTabCarReadable(int sem_id);
int cmpfunct(TResults *a, TResults *b);
typedef int (*compfn)(const void*, const void*);

void endRace(int sig);

// Main server function
void server();




#endif