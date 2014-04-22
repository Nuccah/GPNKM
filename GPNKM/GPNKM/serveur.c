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

void server(int queue_id, int size, int pfdSrvDrv, int pfdDrvSrv, int *drivers, TmsgbufAdr adr_msg){
	int i;
	const char *weather = randomWeather(); // Weather Selection
	//printf("Weather: %s \n", weather);
	for(i=1;i<size;i++){ // Write in pipe all available numbers
		write(pfdSrvDrv, &drivers[i-1], sizeof(int));
	}
	for(i=1;i<size;i++){ // Write in pipe all available numbers
		read(pfdDrvSrv, &adr_msg.tabD[i-1], sizeof(pid_t));
	}
	adr_msg.tabD[22] = getpid();	
	adr_msg.mtype = ADR;
	msgsnd(queue_id, &adr_msg, sizeof(struct TmsgbufAdr), 0);
}
