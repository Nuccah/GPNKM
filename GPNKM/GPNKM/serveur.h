#ifndef H_SERVEUR
#define H_SERVEUR
#include "structures.h"

int randomWeather();
double lapTime(TSect *tabSect);
double globalAverageSpeed(TCar pilot);
double lapAverageSpeed(TLap lap);
bool isTabCarReadable(int sem_id);

void endRace(int sig);

// Main server function
void server(char *date_time);




#endif