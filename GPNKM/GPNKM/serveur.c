#include "serveur.h"

// Chooses random weather condition. Returns weather condition in string form
int randomWeather(int queue_id, pid_t *tabD){
	srand(time(NULL));
	int number = (rand() % (7-1)) + 1;
	int i;
	TmsgbufServ weatherInfo;
	if(number >= 4) number = 3;
	else if(number >= 2) number = 2;
	for(i=0; i<(DRIVERS-1); i++){
		weatherInfo.mtype = tabD[i];
		weatherInfo.mInt = number;
		msgsnd(queue_id, &weatherInfo, sizeof(struct msgbufServ), 0);
	}
	return number;
}

// Sector times as parameters
// Calculates & Returns total lap time
double lapTime(double s1, double s2, double s3){
    return (s1 + s2 + s3);
}

void server(int queue_id, int pfdSrvDrv, int pfdDrvSrv, TmsgbufAdr adr_msg, TCar *tabCar, int sem_id){
	int i;
	int drivers[] = {1,3,6,7,8,20,11,21,25,19,4,9,44,14,13,22,27,99,26,77,17,10}; // Tableau contenant les #'s des conducteurs
	int size = (sizeof(drivers) / sizeof(int))+1;
	//printf("Weather: %s \n", weather);
	for(i=1;i<size;i++){ // Write in pipe all available numbers
		write(pfdSrvDrv, &drivers[i-1], sizeof(int));
	}
	for(i=1;i<size;i++){ // Read in pipe the PID of each drivers and stock in table
		read(pfdDrvSrv, &adr_msg.tabD[i-1], sizeof(pid_t));
	}
	adr_msg.mtype = ADR;
	adr_msg.tabD[22] = getpid();	
	adr_msg.weather = randomWeather(queue_id, adr_msg.tabD); // Weather Selection, Write on MQ for everyone the weather
	msgsnd(queue_id, &adr_msg, sizeof(struct TmsgbufAdr), 0);
	TCar tabRead[22];
	do {
		sleep(2);
		int k;
		printf("\n\n[Server] begins table read!!\n\n");
		for(k = 0; k < 22; k++){
			if(isTabCarReadable(sem_id)) tabRead[k] = tabCar[k];
			else k--;
			printf("\n\n[Server] Tires and speed for car %d: %d - %.2lf\n\n", 
					tabRead[k].num, tabRead[k].tires, tabRead[k].avgSpeed); 
		}
		printf("\n\n[Server] table read done!!\n\n");
	} while(1);
}

// Check if shared mem is readable
bool isTabCarReadable(int sem_id){
	if(semctl(sem_id, 0, GETVAL, 1) == 1) return true;
	return false;
}
