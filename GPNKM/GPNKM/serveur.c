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

	// Read race type from Monitor
	int type = 0;
	while(!((type >= TR1) && (type <= GP))){ 
		while(!isShMemReadable(sem_DispSrv, DISP_WRITE));
		semDown(sem_DispSrv, SRV_WRITE);
		type = listStock->type;
		semUp(sem_DispSrv, SRV_WRITE);
	}
	// Write race type 
	semDown(sem_type, 0);
	*raceType = type;
	semUp(sem_type, 0);

	TResults tabResult[22];

	// Wait for drivers ready
	for(i = 0; i < 22; i++){
		if(isShMemReadable(sem_race, i)) 
		{
			tabRead[i] = tabCar[i];
			tabResult[i].teamName = tabRead[i].teamName;
			tabResult[i].num = tabRead[i].num;
			tabResult[i].timeGlobal = 0;
		}
		else i--;
		if(!tabRead[i].ready) i--;
	}

	TBest bestDriver;
	bestDriver.time = 0;
	semDown(sem_start, 0);
	do {
		int k;
		for(k = 0; k < 22; k++){
			if(isShMemReadable(sem_race, k))
			{
				tabRead[k] = tabCar[k];
				tabResult[k].lnum = tabRead[k].lnum;
				tabResult[k].timeLastLap = lapTime(tabRead[k].lapTimes[tabRead[k].lnum].tabSect);
				tabResult[k].timeGlobal += tabResult[k].timeLastLap;
				tabResult[k].retired = tabRead[k].retired;
				tabResult[k].pitstop = tabRead[k].pitstop;

				if(bestDriver.time > tabResult[k].timeLastLap) // if best lap time is bigger than timeLastLap 
				{
					bestDriver.time = tabResult[k].timeLastLap;
					bestDriver.teamName = tabResult[k].teamName;
					bestDriver.num = tabResult[k].num;
				}
			} 
			else k--;
		}	    
	} while(1);
}

// Sector table as parameter
// Calculate & return lap time 
double lapTime(TSect *tabSect){
    return (tabSect[1].stime + tabSect[2].stime + tabSect[3].stime);
}


