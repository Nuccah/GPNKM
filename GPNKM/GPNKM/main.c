#include "structures.h"

int randomWeather(int min, int max);
double randomNumber(double min, double max);
const char * getTeamName(int number);

int main (int argc, char *argv[])
{
	printf ("Welcome to the worldest famous GPNKM!\n");
	printf("Speed: %.2lf \n", randomNumber(285.1, 310.0));
	const char *team = getTeamName(99);
	printf("Name: %s \n", team);
	return 0;
}

int randomWeather(int min, int max){
	srand(time(NULL));
	return (rand() % (max-min) + min);
}

double randomNumber(double min, double max){
	srand(time(NULL));
	double range = (max - min); 
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

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

