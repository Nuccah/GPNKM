#ifndef H_SERVEUR
#define H_SERVEUR
#include "structures.h"

int randomWeather(int queue_id, pid_t *tabD);
double lapTime(double s1, double s2, double s3);
bool isTabCarReadable(int sem_id);
void server(int queue_id, int pfdSrvDrv, int pfdDrvSrv, TmsgbufAdr adr_msg, 
			TCar *tabCar, int sem_race);




#endif