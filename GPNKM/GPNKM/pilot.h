#ifndef H_PILOT
#define H_PILOT
#include "structures.h"
#include "constants.h"

bool crashed();
bool damaged();
int chooseTires(int weather, TCar pilot);
double tireWear(int weather);
double fuelConsumption();
double fuelStart();
double randomNumber(double min, double max);
double speedWeather(const char *weather);
double sectorTime(double speed, int sector);
const char * getTeamName(int number);
int forkPilots(int pfdSrvDrv, int pfdDrvSrv);
void pilot(int number,int queue_id,int pfdSrvDrv,int pfdDrvSrv, TmsgbufPilot pilot_msg);

#endif