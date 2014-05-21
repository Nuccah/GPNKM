#include "pilot.h"

int forkPilots(int queue_id, int pfdSrvDrv, int pfdDrvSrv, TmsgbufPilot pilot_msg, 
			   TCar *tabCar, int sem_race, int *raceType, int sem_type, int sem_start){
	int i;
	pid_t pid; // ????
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
          	pilot(number, queue_id, pfdSrvDrv, pfdDrvSrv, pilot_msg, i, pidNum, 
          		  tabCar, sem_race, raceType, sem_type, sem_start);
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

void trial(int totalTime, TCar *tabCar, int sem_race, int numCell, TCar *pilot, int sem_start, int weatherFactor)
{
	pilot->lapTimes = malloc(150*sizeof(TLap)); // Alloc sufficient laps for trial
	sendReady(tabCar, sem_race, numCell, pilot); // Send ready to the server
	do
	{
		while(!isShMemReadable(sem_start, 0)); 
	} while(semctl(sem_start, 0, GETVAL) != 0); // Wait for the start sig

	int i = 0;
	int lap = 0;
	double tireStatus = 100;
	while(totalTime > 0)
	{
		if(i = 3)
		{
			i = 0;
			lap++;
		} 
		pilot->lapTimes[lap].tabSect[i].speed = speedWeather(weatherFactor);
		pilot->lapTimes[lap].tabSect[i].stime = sectorTime(pilot->lapTimes[lap].tabSect[i].speed, i);
		pilot->damaged = damaged();
		pilot->crashed = crashed();

		pilot->fuelStock = pilot->fuelStock - fuelConsumption();
		if(pilot->fuelStock <= 0) pilot->retired = true;

		tireStatus = tireStatus - tireWear(weatherFactor);
		if(tireStatus <= TIREWEARLIMIT){
			pilot->tires = pilot->tires - 1;
			if (pilot->tires < 0) pilot->retired = true;
			tireStatus = 100;
		}

		i++;
		semDown(sem_race, numCell);
		tabCar[numCell] = *pilot;
		semUp(sem_race, numCell);
	}
	sendOver(tabCar, sem_race, numCell, pilot);
}

void pilot(int number, int queue_id, int pfdSrvDrv, int pfdDrvSrv, TmsgbufPilot pilot_msg, 
			int numCell, pid_t pid, TCar *tabCar, int sem_race, int *raceType, int sem_type,
			int sem_start){	
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
		while(!((race >= TR1) && (race <= GP))) // Wait for the race type
		{
			while(!isShMemReadable(sem_type, 0));
			race = *raceType;
		}
		switch(race)
		{
			case TR1: trial(5400, tabCar, sem_race, numCell, &pilot, sem_start, weatherInfo.mInt);
					break;
			case TR2: trial(5400, tabCar, sem_race, numCell, &pilot, sem_start, weatherInfo.mInt);
					break;
			case TR3: trial(3600, tabCar, sem_race, numCell, &pilot, sem_start, weatherInfo.mInt);
					break;
			case QU1:
					break;
			case QU2:
					break;
			case QU3: 
					break;
			case GP:
					break;
		}
	}while(1);
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

// Function that returns fuel consumption in liters between 0.3L & 0.7L per sector
double fuelConsumption(){
    return randomNumber(FUELCMIN, FUELCMAX);
}

double fuelStart(){
    return randomNumber(FUELSMIN, FUELSMAX);
}

double tireWear(int weather){
    switch( weather ) {
    	case 1: return randomNumber(TIREWEARMIN, TIREWEARMAX);
    	case 2: return randomNumber((TIREWEARMIN*WETFACTOR), (TIREWEARMAX*WETFACTOR));
    	case 3: return randomNumber((TIREWEARMIN*DRYFACTOR), (TIREWEARMAX*DRYFACTOR));
	}
}

// Speed and Weather (in string form) as parameters
// Returns modified speed according to weather
double speedWeather(int weather){
    double speed = randomNumber(MINSPEED, MAXSPEED);
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

