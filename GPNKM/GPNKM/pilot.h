#ifndef H_PILOT
#define H_PILOT
#include "structures.h"

bool crashed();
bool damaged();
bool tiresWorn(double tireStatus);
int chooseTires(int weather);
double tireWear(double tirestatus, int weather);
double fuelConsumption(int fuelStock);
double fuelStart();
double randomNumber(double min, double max);
double speedWeather(int weather);
double sectorTime(double speed, int sector);
const char * getTeamName(int number);
int getPitstop(int number);
bool enterPitstop(int num, int tabPitstop, int sem_pitstop);
bool exitPitstop(int num, int tabPitstop, int sem_pitstop);
int forkPilots(int queue_id, int pfdSrvDrv, int pfdDrvSrv, TmsgbufPilot pilot_msg,
			  TCar *tabCar, int sem_race, int *raceType, int sem_type, int sem_start);
void pilot(int number, int queue_id, int pfdSrvDrv, int pfdDrvSrv, TmsgbufPilot pilot_msg, 
			int numCell, pid_t pid, TCar *tabCar, int sem_race, int *raceType, int sem_type, int sem_start);
/*void pilot(int number, int queue_id, int pfdSrvDrv, int pfdDrvSrv, TmsgbufPilot pilot_msg, 
			int table, pid_t pid, TCar *tabCar, int sem_race, int *raceType, int sem_type, int tabPitstop, int sem_pitstop);*/

#endif