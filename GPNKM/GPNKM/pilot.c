#include "pilot.h"

int forkPilots(int queue_id, int pfdSrvDrv, int pfdDrvSrv, TmsgbufPilot pilot_msg, int sem_type, int sem_control,
			   int sem_race, int shm_race){
	int i;
	pid_t pid; // ????
	/*SEMA PITSTOP INIT*/
	////
	key_t sem_pitstop_key = ftok(PATH, 'Z'); // Sema Key generated
	int sem_pitstop = semget(sem_pitstop_key, 11, IPC_CREAT | PERMS); // sema ID containing 22 physical sema!!
	semctl(sem_pitstop, 0, SETVAL, 1); // init all sema's at 1
	for(i = 0; i < 11; i++)
	{
		semctl(sem_pitstop, i, SETVAL, 1);  // init all sema's at 1
	}
	////
	/*SHARED PITSTOP MEM INIT*/
	////
	key_t shm_pitstop_key = ftok(PATH, 'X');
	int shm_pitstop = shmget(shm_pitstop_key, 11*sizeof(bool), IPC_CREAT | PERMS); // Creation Pitstop Shared Memory
	bool *tabPitstop = (void *) shmat(shm_pitstop, NULL, 0); // Creation table shared and pilots
	for(i=0;i<11;i++)
	{
		tabPitstop[i] = false;
	}
	for(i=0;i<DRIVERS;i++){ // Multifork des 22 pilotes
		pid = fork();

      	if(pid == -1){
         	printf("Error while attempting Fork (Pilot/Pilot)");
          	exit(EXIT_FAILURE);
        }
      	if(pid==0){ // DRIVERS //
			int number;
			int pidNum = getpid();
			srand((pidNum*10)+time(NULL));
          	read(pfdSrvDrv, &number, sizeof(int)); // First come first serve for driver numbers in pipe
          	write(pfdDrvSrv, &pidNum, sizeof(int)); // Write in pipe pilots PID for later kill
          	pilot(number, queue_id, pfdSrvDrv, pfdDrvSrv, pilot_msg, sem_type, sem_control,
			sem_race, shm_race, i, pidNum, tabPitstop, sem_pitstop);
       	}
    }
	int status = 0;
	waitpid(pid, &status, 0);
	exit(EXIT_SUCCESS);
}

void sendReady(TCar *tabCar, int sem_race, int numCell, TCar *pilot)
{
	pilot->ready = true;
	semDown(sem_race, numCell);
	tabCar[numCell] = *pilot;
	semUp(sem_race, numCell);
}

void sendOver(TCar *tabCar, int sem_race, int numCell, TCar *pilot)
{
	pilot->ready = false;
	semDown(sem_race, numCell);
	tabCar[numCell] = *pilot;
	semUp(sem_race, numCell);	
}

void trial(int totalTime, TCar *tabCar, int sem_race, int numCell, TCar *pilot, int sem_control, int weatherFactor, 
			bool *tabPitstop, int sem_pitstop)
{
	pilot->lapTimes = malloc(150*sizeof(TLap)); // Alloc sufficient laps for trial
	semDown(sem_race, 0);
	tabCar[numCell] = *pilot;
	semUp(sem_race, 0);
	waitSig(SIGSTART, sem_control, 0); // Wait for start signal

	int i = 0;
	int lap = 0;
	bool isDamaged = false;
	bool finished = false;
	pilot->avgSpeed = 0.0;
	double tireStatus = 100;
	while(!finished)
	{
		if(i = 3)
		{
			i = 0;
			pilot->lnum = lap;
			lap++;
		} 
		pilot->lapTimes[lap].tabSect[i].speed = speedWeather(weatherFactor, isDamaged);
		pilot->lapTimes[lap].tabSect[i].stime = sectorTime(pilot->lapTimes[lap].tabSect[i].speed, i);
		isDamaged = damaged();
		if(isDamaged) pilot->crashed = crashed();
		if(pilot->crashed) pilot->retired = true; 
		else
		{
			pilot->fuelStock = fuelConsumption(pilot->fuelStock);
			if(pilot->fuelStock <= 0) pilot->retired = true;
			tireStatus = tireWear(tireStatus, weatherFactor);
			if(i=2){
				if(tiresWorn(tireStatus) || isDamaged){
					if(enterPitstop(pilot->num, tabPitstop, sem_pitstop))
					{
						pilot->lapTimes[lap].tabSect[i].stime = pilot->lapTimes[lap].tabSect[i].stime + pitTime();
						if(tiresWorn(tireStatus))
						{
							pilot->tires = pilot->tires - 1;
							if (pilot->tires < 0) pilot->retired = true;
							tireStatus = 100;
							pilot->lapTimes[lap].tabSect[i].stime = pilot->lapTimes[lap].tabSect[i].stime + changeTime();
						}
						if(isDamaged)
						{
							pilot->lapTimes[lap].tabSect[i].stime = pilot->lapTimes[lap].tabSect[i].stime + repairTime();
							isDamaged = false;
						}
					}
				}
			}
		}
		i++;
		semDown(sem_race, numCell);
		tabCar[numCell] = *pilot;
		semUp(sem_race, numCell);
		if (pilot->retired) break;
		if (checkSig(SIGEND, sem_control, 0)) finished = true;
	}
	sendOver(tabCar, sem_race, numCell, pilot);
}

void pilot(int number, int queue_id, int pfdSrvDrv, int pfdDrvSrv, TmsgbufPilot pilot_msg,  int sem_type, int sem_control,
			int sem_race, int shm_race, int numCell, pid_t pid, bool *tabPitstop, int sem_pitstop){	
	// INIT SECTION
	TCar *tabCar = (void *)shmat(shm_race, NULL, 0);

	TmsgbufServ weatherInfo;
	TCar pilot;
	pilot.num = number;
	pilot.teamName = getTeamName(pilot.num); 
	pilot_msg.mtype = SERVER;
	pilot_msg.car = pilot;
	pilot.fuelStock = fuelStart();
	msgsnd(queue_id, &pilot_msg, sizeof(struct msgbufPilot), 0);
	msgrcv(queue_id, &weatherInfo, sizeof(struct msgbufServ), pid, 0);
	pilot.tires = chooseTires(weatherInfo.mInt);
	char *env;
	sprintf(env, "Pilot %d", getpid());
	show_debug(env, "Initialisation complete!");

	do{
		int race = 0;
		while(!((race >= SIGTR1) && (race <= SIGGP))) race = getSig(sem_type, 0);
		printf("Driver %d - Race tpye: %d\n", getpid(), race);
		switch(race)
		{
			case SIGTR1: trial(5400, tabCar, sem_race, numCell, &pilot, sem_control, weatherInfo.mInt, tabPitstop, sem_pitstop);
					break;
			case SIGTR2: trial(5400, tabCar, sem_race, numCell, &pilot, sem_control, weatherInfo.mInt, tabPitstop, sem_pitstop);
					break;
			case SIGTR3: trial(3600, tabCar, sem_race, numCell, &pilot, sem_control, weatherInfo.mInt, tabPitstop, sem_pitstop);
					break;
			case SIGQU1:
					break;
			case SIGQU2:
					break;
			case SIGQU3: 
					break;
			case SIGGP:
					break;
		}
	}while(!checkSig(SIGEXIT, sem_control, 0));
	shmdt(&shm_race);
}

bool crashed(){
	return ((rand()/(RAND_MAX+1.0)) < CRASH);
}

bool damaged(){
	return ((rand()/(RAND_MAX+1.0)) < BREAK);
}

int chooseTires(int weather){
	switch( weather ) {
    	case 1: return WETS;
    	case 2: return INTERMEDIATES;
    	case 3: return SLICKS;
	}
}

// Random number function
double randomNumber(double min, double max){
	double range = (max - min); 
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

double pitTime()
{
    return randomNumber(MINTIME, MAXTIME);
}

double changeTime()
{
    return randomNumber(MINCHANGE, MAXCHANGE);
}

double repairTime()
{
    return randomNumber(MINREPAIR, MAXREPAIR);
}

// Function that returns fuel consumption in liters between 0.3L & 0.7L per sector
double fuelConsumption(int fuelStock){
    return (fuelStock - randomNumber(FUELCMIN, FUELCMAX));
}

double fuelStart(){
    return randomNumber(FUELSMIN, FUELSMAX);
}

double tireWear(double tireStatus, int weather){
    switch( weather ) {
    	case 1: return (tireStatus - randomNumber(TIREWEARMIN, TIREWEARMAX));
    	case 2: return (tireStatus - randomNumber((TIREWEARMIN*WETFACTOR), (TIREWEARMAX*WETFACTOR)));
    	case 3: return (tireStatus - randomNumber((TIREWEARMIN*DRYFACTOR), (TIREWEARMAX*DRYFACTOR)));
	}
}

// FUNCTION RETURNS BOOLEAN OF WHETHER TIRES NEED TO BE REPLACED OR NOT
bool tiresWorn(double tireStatus){
	if(tireStatus <= TIREWEARLIMIT) return true;
	return false;
}

// Speed and Weather (in string form) as parameters
// Returns modified speed according to weather
double speedWeather(int weather, bool isDamaged){
	double speed;
	if(isDamaged) speed = DAMAGEFACTOR * randomNumber(MINSPEED, MAXSPEED);
    else speed = randomNumber(MINSPEED, MAXSPEED);
    double factor;
    switch(weather){
    	case 3: factor = DRY;
    			break;
    	case 2: factor = WET;
				break;
		case 1: factor = RAIN;
				break;
	}
    return (speed * factor);
}

// Car speed and sector number as parameters
// Calculates & returns sector time
double sectorTime(double speed, int sector){
    double mps = (((speed * 1000) / 60) / 60); // Speed in KPH (KM per Hour) to MPS (Meters per Second)
    switch(sector)
    {
    	case 0: return (S1 / mps);
    	case 1: return (S2 / mps);
    	case 2: return (S3 / mps);
    	default: return 0.0;
    }
}

// Attributed car number as parameter
// Returns Team Name as String
const char * getTeamName(int number){
    char *x;
	switch( number ) {
    	case 1: case 3: x = "Red Bull Racing-Renault"; break;
    	case 6: case 44: x = "Mercedes"; break;
    	case 7: case 14: x = "Ferrari"; break;
    	case 8: case 13: x = "Lotus-Renault"; break;
    	case 20: case 22: x = "McLaren-Mercedes"; break;
    	case 11: case 27: x = "Force India-Mercedes"; break;
    	case 21: case 99: x = "Sauber-Ferrari"; break;
    	case 25: case 26: x = "STR-Renault"; break;
    	case 19: case 77: x = "Williams-Mercedes"; break;
    	case 4: case 17: x = "Marussia-Ferrari"; break;
    	case 9: case 10: x = "Caterham-Renault"; break;
	}
	return x;
}

int getPitstop(int number){
    int x;
	switch( number ) {
    	case 1: case 3: x = 0; break;
    	case 6: case 44: x = 1; break;
    	case 7: case 14: x = 2; break;
    	case 8: case 13: x = 3; break;
    	case 20: case 22: x = 4; break;
    	case 11: case 27: x = 5; break;
    	case 21: case 99: x = 6; break;
    	case 25: case 26: x = 7; break;
    	case 19: case 77: x = 8; break;
    	case 4: case 17: x = 9; break;
    	case 9: case 10: x = 10; break;
	}
	return x;
}

bool enterPitstop(int num, bool *tabPitstop, int sem_pitstop)
{
	int tab = getPitstop(num);
	if(isShMemReadable(sem_pitstop, 0))
	{
		semDown(sem_pitstop, tab);
		tabPitstop[tab] = true;
		semUp(sem_pitstop, tab);
		return true;
	}
	return false;
}

bool exitPitstop(int num, bool *tabPitstop, int sem_pitstop)
{
	int tab = getPitstop(num);
	if(isShMemReadable(sem_pitstop, 0))
	{
		semDown(sem_pitstop, tab);
		tabPitstop[tab] = false;
		semUp(sem_pitstop, tab);
		return true;
	}
	return false;
}
/*
void race(int raceType, TCar *tabCar, int sem_race, int numCell, TCar *pilot, int sem_start, int weatherFactor, 
			bool *tabPitstop, int sem_pitstop)
{
	switch(race)
	{
		case TR1:
		case TR2: int totalTime = 5400; pilot->lapTimes = malloc(150*sizeof(TLap));
				break;
		case TR3: int totalTime = 3600; pilot->lapTimes = malloc(125*sizeof(TLap));
				break;
		case QU1: int totalTime = 1800; pilot->lapTimes = malloc(100*sizeof(TLap));
				break;
		case QU2: int totalTime = 600; pilot->lapTimes = malloc(50*sizeof(TLap));
				break;
		case QU3: int totalTime = 720; pilot->lapTimes = malloc(75*sizeof(TLap));
				break;
		case GP: pilot->lapTimes = malloc(54*sizeof(TLap)); break;
	}

	pilot->lapTimes = malloc(150*sizeof(TLap)); // Alloc sufficient laps for trial
	sendReady(tabCar, sem_race, numCell, pilot); // Send ready to the server
	do
	{
		while(!isShMemReadable(sem_start, 0)); 
	} while(semctl(sem_start, 0, GETVAL) != 0); // Wait for the start sig

	int i = 0;
	int lap = 0;
	bool isDamaged;
	bool finished = false;
	pilot->avgSpeed = 0.0;
	double tireStatus = 100;
	while(!finished)
	{
		if(i = 3)
		{
			i = 0;
			pilot->lnum = lap+1;
			lap++;
		} 
		pilot->lapTimes[lap].tabSect[i].speed = speedWeather(weatherFactor, isDamaged);
		pilot->lapTimes[lap].tabSect[i].stime = sectorTime(pilot->lapTimes[lap].tabSect[i].speed, i);
		isDamaged = damaged();
		if(isDamaged) pilot->crashed = crashed();
		if(pilot->crashed) pilot->retired = true; 
		else
		{
			pilot->fuelStock = fuelConsumption(pilot->fuelStock);
			if(pilot->fuelStock <= 0) pilot->retired = true;
			tireStatus = tireWear(tireStatus, weatherFactor);
			if(i=2){
				if(tiresWorn(tireStatus) || isDamaged){
					if(enterPitstop(pilot->num, tabPitstop, sem_pitstop))
					{
						pilot->lapTimes[lap].tabSect[i].stime = pilot->lapTimes[lap].tabSect[i].stime + pitTime();
						if(tiresWorn(tireStatus))
						{
							pilot->tires = pilot->tires - 1;
							if (pilot->tires < 0) pilot->retired = true;
							tireStatus = 100;
							pilot->lapTimes[lap].tabSect[i].stime = pilot->lapTimes[lap].tabSect[i].stime + changeTime();
						}
						if(isDamaged)
						{
							pilot->lapTimes[lap].tabSect[i].stime = pilot->lapTimes[lap].tabSect[i].stime + repairTime();
							isDamaged = false;
						}
					}
				}
			}
		}
		i++;
		semDown(sem_race, numCell);
		tabCar[numCell] = *pilot;
		semUp(sem_race, numCell);
		if (pilot->retired) break;
		switch(race)
		{
			case TR1:
			case TR2: if (time >= totalTime) finished=true;
					break;
			case TR3: if (time >= totalTime) finished=true;
					break;
			case QU1: if (time >= totalTime) finished=true;
					break;
			case QU2: if (time >= totalTime) finished=true;
					break;
			case QU3: if (time >= totalTime) finished=true;
					break;
			case GP:  if (lap+1=GPLAPS) finished=true;
					break;
		}
	}
	sendOver(tabCar, sem_race, numCell, pilot);
}
*/

