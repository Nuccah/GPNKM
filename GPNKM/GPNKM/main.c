#include "structures.h"

int randomWeath(int min, int max);
double randomNumber(double min, double max);

int main (int argc, char *argv[])
{
	printf ("Welcome to the worldest famous GPNKM!\n");
	printf("Speed: %.2lf \n", randomNumber(285.1, 310.0));

	return 0;
}

int randomWeath(int min, int max){
	srand(time(NULL));
	return (rand() % (max-min) + min);
}

double randomNumber(double min, double max){
	srand(time(NULL));
	double range = (max - min); 
    double div = RAND_MAX / range;
    return min + (rand() / div);
}


