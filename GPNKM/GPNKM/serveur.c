#include "serveur.h"

// Chooses random weather condition. Returns weather condition in string form
const char * randomWeather(){
	srand(time(NULL));
	int number = (rand() % (7-1)) + 1;
	char *x;
	switch( number ) {
    	case 1: x = "RAIN"; break;
    	case 2:case 3: x = "WET"; break;
    	case 4:case 5:case 6: x = "DRY"; break;
	}
	return x;
}

// Sector times as parameters
// Calculates & Returns total lap time
double lapTime(double s1, double s2, double s3){
    return (s1 + s2 + s3);
}
