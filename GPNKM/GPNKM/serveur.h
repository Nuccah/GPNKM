#ifndef H_SERVEUR
#define H_SERVEUR
#include "structures.h"
#include "constants.h"

const char * randomWeather();
double lapTime(double s1, double s2, double s3);
void server(int queue_id, int size, int pfdSrvDrv, int pfdDrvSrv, int *drivers, TmsgbufAdr adr_msg);




#endif