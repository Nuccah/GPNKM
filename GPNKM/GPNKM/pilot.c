#include "pilot.h"

int forkPilots(int queue_id, int pfdSrvDrv, int pfdDrvSrv, TmsgbufPilot pilot_msg, TCar *tabCar, int sem_id){
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
          	pilot(number, queue_id, pfdSrvDrv, pfdDrvSrv, pilot_msg, i, pidNum, tabCar, sem_id);
       	}
    }
	int status = 0;
	waitpid(pid, &status, 0);
	exit(EXIT_SUCCESS);
}

void pilot(int number, int queue_id, int pfdSrvDrv, int pfdDrvSrv, TmsgbufPilot pilot_msg, 
			int table, pid_t pid, TCar *tabCar, int sem_id){	
	TmsgbufServ weatherInfo;
	TCar pilot = tabCar[table];
	pilot.num = number;
	pilot.teamName = getTeamName(pilot.num); 
	pilot_msg.mtype = SERVER;
	pilot_msg.car = pilot;
	pilot.fuelStock = fuelStart(pid);
	msgsnd(queue_id, &pilot_msg, sizeof(struct msgbufPilot), 0);
	msgrcv(queue_id, &weatherInfo, sizeof(struct msgbufServ), pid, 0);
	pilot.tires = chooseTires(weatherInfo.mInt);
	semDown(sem_id);
	tabCar[table] = pilot;
	semUp(sem_id);
	double tireStatus = 100;
	int i = 0;
	do{
		sleep(2);
		if (damaged()){
			pilot.retired = true;
			printf("%d : Car damaged, retiring\n", pid);
			break;
		}
		if (crashed()){
			pilot.crashed = true;
			printf("%d : Car crashed. PILOT IS DEAD AHHHH!!!!\n", pid);
			break;
		}
		pilot.fuelStock = pilot.fuelStock - fuelConsumption();
		if(pilot.fuelStock <= 0){
			pilot.retired = true;
			printf("%d : Car out of fuel, retiring\n", pid);
			break;
		}
		tireStatus = tireStatus - tireWear(weatherInfo.mInt);
		if(tireStatus <= TIREWEARLIMIT){
			pilot.tires = pilot.tires - 1;
			if (pilot.tires < 0){
				printf("%d : No more tires. Retiring\n", pid);
				break;
			}
			tireStatus = 100;
			printf("\n%d : Changing tires, %d remaining\n", pid, pilot.tires);
		}
		double speed = speedWeather(weatherInfo.mInt);
		pilot.avgSpeed = speed;
		printf("%d : Car Survived Lap %d with %.2lf of fuel remaining\n"
				"\tSector time: %.2lf and speed: %.2lf\n", pid, i, pilot.fuelStock, sectorTime(speed, i), speed);
		i++;
		semDown(sem_id);
		tabCar[table] = pilot;
		semUp(sem_id);
	}while(i < 165);
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

