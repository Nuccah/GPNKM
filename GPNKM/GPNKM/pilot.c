#include "pilot.h"

int forkPilots(int pfdSrvDrv, int pfdDrvSrv){
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
          	read(pfdSrvDrv, &number, sizeof(int)); // First come first serve for driver numbers in pipe
          	write(pfdDrvSrv, &pidNum, sizeof(int)); // Write in pipe pilots PID for later kill
          	return number;
       	}
    }
	int status = 0;
	waitpid(pid, &status, 0);
	exit(EXIT_SUCCESS);
}

void pilot(int number, int queue_id, int pfdSrvDrv, int pfdDrvSrv, TmsgbufPilot pilot_msg){
	TmsgbufServ weatherInfo;
	pid_t pid = getpid();
	struct TCar pilot = {0};
	pilot.num = number; 
	pilot_msg.mtype = SERVER;
	pilot_msg.car = pilot;
	pilot.fuelStock = fuelStart(pid);
	msgsnd(queue_id, &pilot_msg, sizeof(struct msgbufPilot), 0);
	msgrcv(queue_id, &weatherInfo, sizeof(struct msgbufServ), pid, 0);
	pilot.tires = chooseTires(weatherInfo.mInt, pilot);
	int i = 0;
	do{
		sleep(1);
		if (damaged(pid)){
			pilot.retired = true;
			printf("%d : Car damaged, retiring\n", pid);
			break;
		}
		if (crashed(pid)){
			pilot.crashed = true;
			printf("%d : Car crashed. PILOT IS DEAD AHHHH!!!!\n", pid);
			break;
		}
		pilot.fuelStock = pilot.fuelStock - fuelConsumption(pid);
		if(pilot.fuelStock <= 0){
			pilot.retired = true;
			printf("%d : Car out of fuel, retiring\n", pid);
			break;
		}
		printf("%d : Car Survived Lap %d with %.2lf of fuel remaining\n", pid, i, pilot.fuelStock);
		i++;
	}while(i < 165);
}

bool crashed(pid_t pid){
	srand(pid+time(NULL));
	return ((rand()/(RAND_MAX+1.0)) < CRASH);
}

bool damaged(pid_t pid){
	srand(pid+time(NULL));
	return ((rand()/(RAND_MAX+1.0)) < BREAK);
}

int chooseTires(int weather, TCar pilot){
	switch( weather ) {
    	case 1: return WETS;
    	case 2:case 3: return INTERMEDIATES;
    	case 4:case 5:case 6: return SLICKS;
	}
}

// Random number function
double randomNumber(double min, double max, pid_t pid){
	srand(pid+time(NULL));
	double range = (max - min); 
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

// Function that returns fuel consumption in liters between 0.3L & 0.7L per sector
double fuelConsumption(pid_t pid){
    srand(pid+time(NULL));
    return (FUELCMIN + (rand() / (RAND_MAX / (FUELCMAX - FUELCMIN))));
}

double fuelStart(pid_t pid){
    srand(pid+time(NULL));
    return (FUELSMIN + (rand() / (RAND_MAX / (FUELSMAX - FUELSMIN))));
}
// Speed and Weather (in string form) as parameters
// Returns modified speed according to weather
double speedWeather(const char *weather, pid_t pid){
    double speed = randomNumber(MINSPEED, MAXSPEED, pid);
	printf("Speed: %.2lf \n", speed);
    double factor;
    if (strcmp(weather, "DRY") == 0)
    	factor = DRY;
	if (strcmp(weather, "WET") == 0)
    	factor = WET;
	if (strcmp(weather, "RAIN") == 0)
    	factor = RAIN;
    return (speed * factor);
}

// Car speed and sector length as parameters
// Calculates & returns sector time
double sectorTime(double speed, int sector){
    double mps = (((speed * 1000) / 60) / 60); // Speed in KPH (KM per Hour) to MPS (Meters per Second)
    return (sector / mps);
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

