#include "structures.h"

const char * randomWeather();
double randomNumber(double min, double max);
const char * getTeamName(int number);
double speedWeather(const char *weather);
double sectorTime(double speed, int sector);
double lapTime(double s1, double s2, double s3);

int main (int argc, char *argv[])
{
	printf ("Welcome to the worldest famous GPNKM!\n");
	const char *weather = randomWeather();
	printf("Weather: %s \n", weather);
	double speed = speedWeather(weather);
	printf("Weather Modified Speed: %.2lf \n", speed);
	const char *team = getTeamName(99);
	printf("Name: %s \n", team);
	double time1 = sectorTime(speed, S1);
	double time2 = sectorTime(speed, S2);
	double time3 = sectorTime(speed, S3);
	printf("Sector 1 Time: %.2lf \n", time1);
	printf("Sector 2 Time: %.2lf \n", time2);
	printf("Sector 3 Time: %.2lf \n", time3);
	printf("Laptime: %.2lf \n", lapTime(time1, time2, time3));
	return 0;
}


// Chooses random weather condition. Returns weather condition in string form
const char * randomWeather(){
	srand(time(NULL));
	int number = (rand() % (4-1)) + 1;
	char *x;
	switch( number ) {
    	case 1: x = "RAIN"; break;
    	case 2: x = "WET"; break;
    	case 3: x = "DRY"; break;
	}
	return x;
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
    int totalSeconds = (sector / mps);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    double time = (minutes + (seconds * 0.01));
    return time;
}

// Sector times as parameters
// Calculates & Returns total lap time
double lapTime(double s1, double s2, double s3){
    int totalSeconds = ((s1 + s2 + s3) * 100);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    double time = (minutes + (seconds * 0.01));
    return time;
}

// Attributed car number as parameter
// Returns Team Name as String
const char * getTeamName(int number){
    const char *x;
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

