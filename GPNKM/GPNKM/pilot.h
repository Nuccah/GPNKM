#ifndef H_PILOT
#define H_PILOT
#include "structures.h"
#include "constants.h"

int chooseTires(int weather, TCar pilot);
bool crashed(pid_t pid);
bool damaged(pid_t pid);
double fuelConsumption(pid_t pid);
double fuelStart(pid_t pid);
double randomNumber(double min, double max, pid_t pid);
double speedWeather(const char *weather, pid_t pid);
double sectorTime(double speed, int sector);
const char * getTeamName(int number);
int forkPilots(int pfdSrvDrv, int pfdDrvSrv);
void pilot(int number,int queue_id,int pfdSrvDrv,int pfdDrvSrv, TmsgbufPilot pilot_msg);

#endif