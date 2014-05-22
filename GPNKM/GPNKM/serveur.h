#ifndef H_SERVEUR
#define H_SERVEUR
#include "structures.h"

int randomWeather(int queue_id, pid_t *tabD);
double lapTime(TSect *tabSect);
double globalAverageSpeed(TCar pilot);
double lapAverageSpeed(TLap lap);
bool isTabCarReadable(int sem_id);

void endRace(int sig);

// Main server function
void server(int queue_id, int pfdSrvDrv, int pfdDrvSrv, TmsgbufAdr adr_msg);




#endif