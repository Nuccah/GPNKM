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

void server(int queue_id, int pfdSrvDrv, int pfdDrvSrv, TmsgbufAdr adr_msg, TCar *tabCar, int sem_race,
    		TSharedStock *listStock, int sem_DispSrv, int *raceType, int sem_type, int sem_start){
	int i;
	char* msg;
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
	show_success("Server", "Initialisation complete");
	int type = 0;
	while(!((type >= TR1) && (type <= GP))){ 
		while(!isShMemReadable(sem_DispSrv, DISP_WRITE));
		semDown(sem_DispSrv, SRV_WRITE);
		type = listStock->type;
		semUp(sem_DispSrv, SRV_WRITE);
	}

	semDown(sem_type, 0);
	*raceType = type;
	semUp(sem_type, 0);

	for(i = 0; i < 22; i++){
		if(isShMemReadable(sem_race, i)) tabRead[i] = tabCar[i];
		else i--;
		if(!tabRead[i].ready) i--;
	}
	semDown(sem_start, 0);
	do {
		sleep(2);
		int k;
		show_debug("Server",  "begins table read!");
		for(k = 0; k < 22; k++){
			if(isShMemReadable(sem_race, k)) tabRead[k] = tabCar[k];
			else k--;
			printf("\n\n[Server] Tires and speed for car %d: %d - %.2lf\n\n", 
					tabRead[k].num, tabRead[k].tires, tabRead[k].avgSpeed); 
		}
		show_debug("Server", "table read done!");
	} while(1);
}


