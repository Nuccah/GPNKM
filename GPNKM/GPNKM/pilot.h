#ifndef H_PILOT
#define H_PILOT
#include "structures.h"

bool crashed();
bool damaged();
bool tiresWorn(double tireStatus);
bool enterPitstop(int num, bool *tabPitstop, int sem_pitstop);
bool exitPitstop(int num, bool *tabPitstop, int sem_pitstop);
const char * getTeamName(int number);
int chooseTires(int weather);
int getPitstop(int number);
double tireWear(double tirestatus, int weather);
double fuelConsumption(int fuelStock);
double fuelStart();
double randomNumber(double min, double max);
double speedWeather(int weather, bool isDamaged);
double sectorTime(double speed, int sector);
double pitTime();
double changeTime();
double repairTime();
int forkPilots(int queue_id, int pfdSrvDrv, int pfdDrvSrv, TmsgbufPilot pilot_msg,
			  TCar *tabCar, int sem_race, int *raceType, int sem_type, int sem_start);
void pilot(int number, int queue_id, int pfdSrvDrv, int pfdDrvSrv, TmsgbufPilot pilot_msg, 
			int numCell, pid_t pid, TCar *tabCar, int sem_race, int *raceType, int sem_type, int sem_start, 
			bool *tabPitstop, int sem_pitstop);
void trial(int totalTime, TCar *tabCar, int sem_race, int numCell, TCar *pilot, int sem_start, int weatherFactor, 
			bool *tabPitstop, int sem_pitstop);

#endif