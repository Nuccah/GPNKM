#ifndef H_PILOT
#define H_PILOT
#include "structures.h"
#include "constants.h"

int chooseTires(int weather, TCar pilot);
bool crashed();
bool damaged();
double randomNumber(double min, double max);
double speedWeather(const char *weather);
double sectorTime(double speed, int sector);
const char * getTeamName(int number);
int forkPilots(int pfdSrvDrv, int pfdDrvSrv);
void pilot(int number,int queue_id,int pfdSrvDrv,int pfdDrvSrv, TmsgbufPilot pilot_msg);

#endif