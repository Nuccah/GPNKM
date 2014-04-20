#include "pilot.h"

void forkPilots(int nPilots, int pfdSrvDrv){
	int i;
	pid_t pid; // ????
	for(i=0;i<(nPilots);i++){ // Multifork des 22 pilotes
		pid = fork();
      	if(pid == -1){
         	printf("Error while attempting Fork (Pilot/Pilot)");
          	exit(EXIT_FAILURE);
        }
      	if(pid==0){ // DRIVERS //
			int number;
          	printf(" \n pid: %3d\n",i+1);
          	read(pfdSrvDrv, &number, sizeof(int)); // First come first serve for driver numbers in pipe
          	const char *team = getTeamName(number); // Return team name according to driver number
          //	printf("Number: %d - Team: %s \n",number, team);
          	return;
       	}
    }
	exit(EXIT_SUCCESS);
}
// Random number function
double randomNumber(double min, double max){
	srand(time(NULL));
	double range = (max - min); 
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

// Speed and Weather (in string form) as parameters
// Returns modified speed according to weather
double speedWeather(const char *weather){
    double speed = randomNumber(MINSPEED, MAXSPEED);
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

