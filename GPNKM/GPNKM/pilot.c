#include "pilot.h"

int forkPilots(){
	int i;
	pid_t pid;
	/*SEMA PITSTOP INIT*/
	key_t sem_pitstop_key = ftok(PATH, PIT); // Sema Key generated
	int sem_pitstop = semget(sem_pitstop_key, 11, IPC_CREAT | PERMS); // sema ID containing 22 physical sema!!
	for(i = 0; i < 11; i++) semReset(sem_pitstop, i);  // init all sema's at 1

	for(i=0;i<DRIVERS;i++){ // Multifork des 22 pilotes
		pid = fork();

      	if(pid == -1){
         	printf("Error while attempting Fork (Pilot/Pilot)");
          	exit(EXIT_FAILURE);
        }
      	if(pid == 0){ // DRIVERS //
			int pidNum = getpid();
			srand((pidNum*10)+time(NULL));
          	pilot(i, pidNum);
       	}
    }
	int status = 0;
	waitpid(pid, &status, 0);
	for(i=0; i<11; i++)	semctl(sem_pitstop, i, IPC_RMID, NULL);
	exit(EXIT_SUCCESS);
}

void startRace(TCar *tabCar, int sem_race, int numCell, TCar *pilot, int sem_control, int weatherFactor, int sem_switch)
{
	// INIT SECTION
	key_t sem_pitstop_key = ftok(PATH, PIT);
	int sem_pitstop = semget(sem_pitstop_key, 11, IPC_CREAT | PERMS);	

	int numPit = getPitstop(pilot->num);
	// END INIT SECTION
	// Send ready to server
	//pilot->lapTimes = malloc(150*sizeof(TLap)); // Alloc sufficient laps for trial
	sendReady(tabCar, sem_race, numCell, pilot); // Send ready to the server
	waitSig(SIGSTART, sem_control, 0); // Wait for start signal
	int i = 0;
	int lap = 0;
	bool isDamaged = false;
	bool finished = false;
	pilot->crashed = false;
	pilot->retired = false;
	pilot->pitstop = false;
	pilot->lnum = 0;
	pilot->avgSpeed = 0.0;
	double pitstopsleep = 0.0;
	double tireStatus = 100.0;
	while(!finished)
	{
		if(pilot->pitstop == true) 
		{
			pilot->pitstop = false;
			exitPitstop(numPit, sem_pitstop);
		}
		if(i == 3)
		{
			i = 0;
			lap += 1;
		} 
		pilot->lnum = lap;
		pilot->snum = i;
		if(!isDamaged) isDamaged = damaged();

		if(isDamaged) pilot->crashed = crashed();

		if(pilot->crashed) pilot->retired = true;
		else
		{
			pilot->lapTimes[lap].tabSect[i].speed = speedWeather(weatherFactor, isDamaged);
			pilot->lapTimes[lap].tabSect[i].stime = sectorTime(pilot->lapTimes[lap].tabSect[i].speed, i);

			sleep(pilot->lapTimes[lap].tabSect[i].stime*0.1);
			tireStatus = tireWear(tireStatus, weatherFactor);
			if(!pilot->retired)
			{
				if((i == 2) && (tiresWorn(tireStatus) || isDamaged)){
					if(enterPitstop(numPit, sem_pitstop))
					{
						// PITSTOP
						pitstopsleep = pitTime();
						if(tiresWorn(tireStatus))
						{
							pilot->tires = pilot->tires - 1;
							if (pilot->tires < 0) pilot->retired = true;

							tireStatus = 100.0;
							pitstopsleep += changeTime();
						}
						if(isDamaged)
						{
							pitstopsleep += repairTime();
							isDamaged = false;
						}
						pilot->pitstop = true;
						sleep(pitstopsleep*0.1);
						pilot->lapTimes[lap].tabSect[i].stime += pitstopsleep;
						// PITSTOP END
					}
				}
			}
		}
		if(DISPMODE == 2){
			if(pilot->num < 10) printf("[Pilot 0%d] ", pilot->num);
			else printf("[Pilot %d] ", pilot->num);
			printf("Damaged? %3s", isDamaged ? "yes" : "no");
			printf(" | Crashed? %3s", pilot->crashed ? "yes" : "no"); 
			if(lap < 10) printf(" | Lap 0%d", lap);
			else printf(" | Lap %d", lap);
			printf(" | Time Secor %d: %5.2lf", (i+1), pilot->lapTimes[lap].tabSect[i].stime); 
			printf(" | Tires out? %3s", (pilot->tires < 0) ? "yes" : "no"); 
			printf(" | Retired? %3s", pilot->retired ? "yes" : "no");
			printf(" | Pitstop? %3s", pilot->pitstop ? "yes" : "no");
			printf(" | PID: %d", getpid());
			printf("\n");			
		}
		semDown(sem_race, numCell);
		memcpy(&tabCar[numCell].snum, &pilot->snum, sizeof(int));
		memcpy(&tabCar[numCell].lnum, &pilot->lnum, sizeof(int));
		memcpy(&tabCar[numCell].lapTimes[lap].tabSect[i].stime, &pilot->lapTimes[lap].tabSect[i].stime, sizeof(double));
		memcpy(&tabCar[numCell].retired, &pilot->retired, sizeof(bool));
		memcpy(&tabCar[numCell].pitstop, &pilot->pitstop, sizeof(bool));
		semUp(sem_race, numCell);
		semSwitch(sem_switch, numCell);
		
		if(pilot->retired) finished = true;

		if(i==2)
		{
			if (checkSig(SIGEND, sem_control, 0)) finished = true;
		}
		i++;
	}
	sendOver(tabCar, sem_race, numCell, pilot);
	semReset(sem_race, numCell);
	semctl(sem_pitstop, numPit, IPC_RMID, NULL);
}

void pilot(int numCell, pid_t pid){	
	// INIT SECTION
	key_t sem_type_key = ftok(PATH, TYPE);
	int sem_type = semget(sem_type_key, 1, IPC_CREAT | PERMS);

	key_t sem_control_key = ftok(PATH, CONTROL);
	int sem_control = semget(sem_control_key, 2, IPC_CREAT | PERMS);

	key_t sem_switch_key = ftok(PATH, SWITCH);
	int sem_switch = semget(sem_switch_key, 22, IPC_CREAT | PERMS);

	key_t sem_race_key = ftok(PATH, RACE);
	int sem_race = semget(sem_race_key, 22, IPC_CREAT | PERMS);

	key_t shm_race_key = ftok(PATH, RACESHM);
	int shm_race = shmget(shm_race_key, 22*sizeof(TCar), S_IWUSR);
	TCar *tabCar = (TCar *)shmat(shm_race, NULL, 0);
	// END INIT SECTION
	int drivers[] = {1,3,6,7,8,20,11,21,25,19,4,9,44,14,13,22,27,99,26,77,17,10}; // Tableau contenant les #'s des conducteurs
	int weather = 1;
	TCar pilot;
	pilot.num = drivers[numCell];
	pilot.teamName = getTeamName(pilot.num); 
	semDown(sem_race, numCell);
	memcpy(&tabCar[numCell].num, &pilot.num, sizeof(int)); 
	memcpy(&tabCar[numCell].teamName, &pilot.teamName, sizeof(const char *));
	semUp(sem_race, numCell);
	semSwitch(sem_switch, numCell);
	do{
		// Wait weather sig from server
		while(!((weather >= SIGDRY) && (weather <= SIGRAIN))) weather = getSig(sem_control, 1);
		pilot.tires = chooseTires(weather);
		int race = 0;
		while((!((race >= SIGTR1) && (race <= SIGGP))) && (!checkSig(SIGEXIT, sem_control, 0))) race = getSig(sem_type, 0);
		if(checkSig(SIGEXIT, sem_control, 0)) goto eop;
		startRace(tabCar, sem_race, numCell, &pilot, sem_control, weather, sem_switch);
	}while(!checkSig(SIGEXIT, sem_control, 0));
	eop:
		shmdt(&shm_race);
		semctl(sem_type, 0, IPC_RMID, NULL);
		semctl(sem_race, numCell, IPC_RMID, NULL);
		semctl(sem_control, 0, IPC_RMID, NULL);
		semctl(sem_control, 1, IPC_RMID, NULL);
		shmctl(shm_race, IPC_RMID, NULL);
		semctl(sem_switch, numCell, IPC_RMID, NULL);
		return;
}

bool crashed(){
	return ((rand()/(RAND_MAX+1.0)) < CRASH);
}

bool damaged(){
	return ((rand()/(RAND_MAX+1.0)) < BREAK);
}

int chooseTires(int weather){
	switch( weather ) {
    	case SIGRAIN: return WETS;
    	case SIGWET: return INTERMEDIATES;
    	case SIGDRY: return SLICKS;
	}
}

unsigned int sectorSleep(double time, double factor){
	return ((unsigned int)(100000*time*factor));
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

double tireWear(double tireStatus, int weather){
    switch( weather ) {
    	case SIGRAIN: return (tireStatus - randomNumber(TIREWEARMIN, TIREWEARMAX));
    	case SIGWET: return (tireStatus - randomNumber((TIREWEARMIN*WETFACTOR), (TIREWEARMAX*WETFACTOR)));
    	case SIGDRY: return (tireStatus - randomNumber((TIREWEARMIN*DRYFACTOR), (TIREWEARMAX*DRYFACTOR)));
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
    	case SIGDRY: factor = DRY;
    			break;
    	case SIGWET: factor = WET;
				break;
		case SIGRAIN: factor = RAIN;
				break;
	}
    return (speed * factor);
}

// Car speed and sector number as parameters
// Calculates & returns sector time
double sectorTime(double speed, int sector){
    double mps = ((speed * 1000) / 3600); // Speed in KPH (KM per Hour) to MPS (Meters per Second)
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

bool enterPitstop(int numPit, int sem_pitstop)
{
	if(isShMemReadable(sem_pitstop, numPit))
	{
		semDown(sem_pitstop, numPit);
		return true;
	}
	return false;
}

void exitPitstop(int numPit, int sem_pitstop)
{
	semUp(sem_pitstop, numPit);
}

void sendReady(TCar *tabCar, int sem_race, int numCell, TCar *pilot)
{
	pilot->ready = true;
	semDown(sem_race, numCell);
	memcpy(&tabCar[numCell], pilot, sizeof(TCar));
	semUp(sem_race, numCell);
}

void sendOver(TCar *tabCar, int sem_race, int numCell, TCar *pilot)
{
	pilot->ready = false;
	semDown(sem_race, numCell);
	memcpy(&tabCar[numCell], pilot, sizeof(TCar));
	semUp(sem_race, numCell);	
}
