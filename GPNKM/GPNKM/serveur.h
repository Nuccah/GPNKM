#ifndef H_SERVEUR
#define H_SERVEUR
#include "structures.h"

int randomWeather(int queue_id, pid_t *tabD);
double lapTime(TSect *tabSect);
double globalAverageSpeed(TCar pilot);
double lapAverageSpeed(TLap lap);
bool isTabCarReadable(int sem_id);
void server(int queue_id, int pfdSrvDrv, int pfdDrvSrv, TmsgbufAdr adr_msg, TCar *tabCar, int sem_race, 
			TSharedStock *listStock, int sem_DispSrv, int *raceType, int sem_type, int sem_start);




#endif