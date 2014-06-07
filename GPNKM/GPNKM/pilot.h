#ifndef H_PILOT
#define H_PILOT
#include "structures.h"

const char * getTeamName(int number);
bool crashed();
bool damaged();
bool tiresWorn(double tireStatus);
bool enterPitstop(int numPit, int sem_pitstop);
void exitPitstop(int numPit, int sem_pitstop);
int chooseTires(int weather);
unsigned int sectorSleep(double time, double factor);
int getPitstop(int number);
int forkPilots();
double tireWear(double tirestatus, int weather);
double randomNumber(double min, double max);
double speedWeather(int weather, bool isDamaged);
double sectorTime(double speed, int sector);
double pitTime();
double changeTime();
double repairTime();
void sendReady(TCar *tabCar, int sem_race, int numCell, TCar *pilot);
void sendOver(TCar *tabCar, int sem_race, int numCell, TCar *pilot);
void startRace(TCar *tabCar, int sem_race, int sem_modif, int numCell, TCar *pilot, int sem_start, int weatherFactor);
void pilot(int numCell, pid_t pid);


#endif