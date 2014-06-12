#include "serveur.h"

// Chooses random weather condition. Returns weather condition in string form
int randomWeather(){
	srand(time(NULL));
	// Random system following rand() % N + m 
	// where N = number of numbers in the range
	// and m = minimal number to start
	return (rand() % 3 + 5); // Here we want a random number between 5 and 7 so N = 3 and m = 5
}

void server(){
    // INIT SECTION
  
    key_t sem_type_key = ftok(PATH, TYPE);
    int sem_type = semget(sem_type_key, 1, IPC_CREAT | PERMS);

    key_t sem_control_key = ftok(PATH, CONTROL);
    int sem_control = semget(sem_control_key, 2, IPC_CREAT | PERMS);

	key_t sem_mutex_key = ftok(PATH, MUTEX);
	int sem_mutex = semget(sem_mutex_key, 1, IPC_CREAT | PERMS);

    key_t sem_DispSrv_key = ftok(PATH, STOCK);
    int sem_DispSrv = semget(sem_DispSrv_key, 1, IPC_CREAT | PERMS);

    key_t sem_race_key = ftok(PATH, RACE);
    int sem_race = semget(sem_race_key, 22, IPC_CREAT | PERMS);

    key_t shm_DispSrv_key = ftok(PATH, STOCKSHM);
    int shm_DispSrv = shmget(shm_DispSrv_key, sizeof(TSharedStock), S_IWUSR);
	TSharedStock *listStock = (TSharedStock *) shmat(shm_DispSrv, NULL, 0);

	key_t shm_race_key = ftok(PATH, RACESHM);
	int shm_race = shmget(shm_race_key, 22*sizeof(TTabCar), S_IWUSR);
	TTabCar *tabCar = (TTabCar *)shmat(shm_race, NULL, 0);

	// File Parameters
	int stream;
	time_t now;
	/* Time Function for filename definition */
	now = time(NULL);
	struct tm *time = localtime(&now);
	char date_time[30];
	strftime( date_time, sizeof(date_time), "%d%m%y_%H%M%S", time );
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH | O_TRUNC;
    
	show_success("Server", "Initialisation complete");
	int o;
	TOut tabOut[22];
	for(o= 0; o<22; o++){	
		tabOut[o].numPilot = -1;
		tabOut[o].numCell = -1; 
	} 
	// END INIT SECTION
	do{
		int i = 0, j;
		TTabCar tabRead[22];
		for(j = 0; j < 22; j++){
			bool selected = true;
			for(i=0; i < 22; i++){
				if(tabOut[i].numCell == j) selected = false;
			}
			if(selected) sendSig(SIGSELECT, sem_race, j);
		} 
	    // Send weather to monitor and pilots
		sendSig(randomWeather(), sem_control, 1);
		// Wait race type from Monitor
		int type = 0;
		while((!((type >= SIGTR1) && (type <= SIGGP))) && (!checkSig(SIGEXIT, sem_control, 0))) type = getSig(sem_type, 0);
		if(checkSig(SIGEXIT, sem_control, 0)) goto eop;
		printf("Server received: %d\n", type);

		TSharedStock localStock;
		localStock.bestDriver.time = 0;

		while(semGet(sem_DispSrv, 0) != 1);
		semDown(sem_DispSrv, 0);
		for(j=0; j < 22; j++){
			listStock->tabResult[j].timeGlobal = 0.0;
			listStock->tabResult[j].timeLastLap = 0.0;
			listStock->tabResult[j].snum = 0;
			listStock->tabResult[j].lnum = 0;
			listStock->tabResult[j].retired = false;
			listStock->tabResult[j].pitstop = false;
		}
		semUp(sem_DispSrv, 0);
		int m;
		// Wait for drivers ready
		for(i = 0; i < 22; i++){ 
			bool selected = true;
			for(m = 0; m < 22; m++){
				if(tabOut[m].numCell == i) selected = false;
			} 
			if(selected){
				do{
					while(semGet(sem_mutex, TMP1) != 1);
					semDown(sem_mutex, TMP1);
				    memcpy(&tabRead[i].teamName, &tabCar[i].teamName, sizeof(char *));
				    memcpy(&tabRead[i].num, &tabCar[i].num, sizeof(int));
				    memcpy(&tabRead[i].ready, &tabCar[i].ready, sizeof(bool));
				    semUp(sem_mutex, TMP1);

					localStock.tabResult[i].teamName = tabRead[i].teamName;
					localStock.tabResult[i].num = tabRead[i].num;
					localStock.tabResult[i].timeGlobal = 0.0;
					localStock.tabResult[i].timeLastLap = 0.0;
					localStock.tabResult[i].lnum = 0;
					localStock.tabResult[i].snum = 0;
					localStock.tabResult[i].bestLapTime = 1000.0;
					localStock.tabResult[i].retired = false;
					localStock.tabResult[i].pitstop = false;
				}while(!tabRead[i].ready);
			}
		}
	    while(semGet(sem_DispSrv, 0) != 1);
		semDown(sem_DispSrv, 0);
		memcpy(listStock, &localStock, sizeof(TSharedStock)); // Put stock content into shared memory
		semUp(sem_DispSrv, 0);
		
		show_notice("Server", "All drivers are ready!");
		
		// Init signal handler if race type based on time
		semReset(sem_control, 0);
		semReset(sem_control, 1);
		// Send start signal
		double timeMax = 0.0;
		int maxCar = 22;
		switch(type){
			case SIGTR1: 
					timeMax = 5400.0;
					sendSig(SIGSTART, sem_control, 0);
					for(i = 0; i < 22; i++)	sendSig(SIGSTART, sem_race, i); 		
					break;
			case SIGTR2: 
					timeMax = 5400.0;
					sendSig(SIGSTART, sem_control, 0);
					for(i = 0; i < 22; i++)	sendSig(SIGSTART, sem_race, i); 
					break;
			case SIGTR3: 
					timeMax = 3600.0;
					sendSig(SIGSTART, sem_control, 0);
					for(i = 0; i < 22; i++)	sendSig(SIGSTART, sem_race, i); 
					break;
			case SIGQU1: 
					timeMax = 1100.0;
					sendSig(SIGSTART, sem_control, 0);
					for(i = 0; i < 22; i++)	sendSig(SIGSTART, sem_race, i); 
					break;
			case SIGQU2: 
					timeMax = 600.0;
					sendSig(SIGSTART, sem_control, 0);
					for(i = 0; i < 22; i++)	{
						bool selected = true;
						for(m = 0; m < 22; m++){
							if(tabOut[m].numCell == i) selected = false;
						} 
						if(selected) sendSig(SIGSTART, sem_race, i);
					} 
					maxCar = 15;
					break;
			case SIGQU3: 
					timeMax = 800.0;
					sendSig(SIGSTART, sem_control, 0);
					for(i = 0; i < 22; i++)	{
						bool selected = true;
						for(m = 0; m < 22; m++){
							if(tabOut[m].numCell == i) selected = false;
						} 
						if(selected) sendSig(SIGSTART, sem_race, i);
					} 
					maxCar = 8;
					break;
			case SIGGP:
					sendSig(SIGSTART, sem_control, 0);
					for(i = 0; i < 22; i++){
						sendSig(SIGSTART, sem_race, tabOut[i].numCell);
						usleep(500);
					}
					break;
		}
		semReset(sem_type, 0);
		bool finished = false;
		bool tabFinished[22];
		bool tabSelected[22];
		for(i = 0; i < 22; i++){
			tabFinished[i] = false;
			bool selected = true;
			for(m = 0; m < 22; m++){
				if(tabOut[m].numCell == i) selected = false;
			}
			tabSelected[i] = selected; 
		}
		int currentLap = 0;
		int tmpLap = 0, tmpSec = 0, k, nbFinished = 0;
		do {
			sleep(0.2);
			for(k = 0; k < 22; k++){
				if((tabSelected[k]) || (type == SIGGP)){
					tmpLap = localStock.tabResult[k].lnum;
					tmpSec = localStock.tabResult[k].snum;

					while(semGet(sem_mutex, TMP1) != 1);
					semDown(sem_mutex, TMP1);
					memcpy(&tabRead[k].lnum, &tabCar[k].lnum, sizeof(int));
					memcpy(&tabRead[k].snum, &tabCar[k].snum, sizeof(int));
					for(i=tmpLap; i <= tabRead[k].lnum; i++){
						memcpy(&tabRead[k].lapTimes[i], &tabCar[k].lapTimes[i], sizeof(TLap));
					}
					memcpy(&tabRead[k].retired, &tabCar[k].retired, sizeof(bool));
					memcpy(&tabRead[k].pitstop, &tabCar[k].pitstop, sizeof(bool));
					semUp(sem_mutex, TMP1);

					localStock.tabResult[k].lnum = tabRead[k].lnum;
					localStock.tabResult[k].snum = tabRead[k].snum;
					if(tmpLap == tabRead[k].lnum)
					{
						if(tmpSec != tabRead[k].snum){
							for(j=tmpSec; j <= tabRead[k].snum; j++)
							{
								
								if(localStock.tabResult[k].snum == 2) {
									localStock.tabResult[k].timeLastLap = lapTime(tabRead[k].lapTimes[tmpLap].tabSect);
									if((localStock.tabResult[k].bestLapTime > localStock.tabResult[k].timeLastLap) &&
										(localStock.tabResult[k].timeLastLap != 0.0)){
										localStock.tabResult[k].bestLapTime = localStock.tabResult[k].timeLastLap;
									}
								}
								localStock.tabResult[k].timeGlobal += tabRead[k].lapTimes[tmpLap].tabSect[j].stime;
							}
						}
					}
					else
					{
						for(i=tmpLap; i <=tabRead[k].lnum; i++){
							if(i == tmpLap){
								for(j=tmpSec; j<=2; j++){
									if(localStock.tabResult[k].snum == 2) {
										localStock.tabResult[k].timeLastLap = lapTime(tabRead[k].lapTimes[i].tabSect);
										if((localStock.tabResult[k].bestLapTime > localStock.tabResult[k].timeLastLap) &&
											(localStock.tabResult[k].timeLastLap != 0.0)){
											localStock.tabResult[k].bestLapTime = localStock.tabResult[k].timeLastLap;
										}
									}
									localStock.tabResult[k].timeGlobal += tabRead[k].lapTimes[i].tabSect[j].stime;
								}
							}
							else if(i == tabRead[k].lnum){
								for(j=0; j<=tabRead[k].snum; j++){
									if(localStock.tabResult[k].snum == 2) {
										localStock.tabResult[k].timeLastLap = lapTime(tabRead[k].lapTimes[i].tabSect);
										if((localStock.tabResult[k].bestLapTime > localStock.tabResult[k].timeLastLap) &&
											(localStock.tabResult[k].timeLastLap != 0.0)){
											localStock.tabResult[k].bestLapTime = localStock.tabResult[k].timeLastLap;
										}
									}
									localStock.tabResult[k].timeGlobal += tabRead[k].lapTimes[i].tabSect[j].stime;
								}
							}
							else{
								for(j=0; j<=2; j++){
									if(localStock.tabResult[k].snum == 2){ 
										localStock.tabResult[k].timeLastLap = lapTime(tabRead[k].lapTimes[i].tabSect);
										if((localStock.tabResult[k].bestLapTime > localStock.tabResult[k].timeLastLap) &&
											(localStock.tabResult[k].timeLastLap != 0.0)){
											localStock.tabResult[k].bestLapTime = localStock.tabResult[k].timeLastLap;
										}
									}
									localStock.tabResult[k].timeGlobal += tabRead[k].lapTimes[i].tabSect[j].stime;
								}
							}
						}
					}
					// Calculate lap time only when the turn is over
					localStock.tabResult[k].retired = tabRead[k].retired;
					localStock.tabResult[k].pitstop = tabRead[k].pitstop;
					if(localStock.bestDriver.time > localStock.tabResult[k].timeLastLap) // if best lap time is bigger than timeLastLap 
					{
						localStock.bestDriver.time = localStock.tabResult[k].timeLastLap;
						localStock.bestDriver.teamName = localStock.tabResult[k].teamName;
						localStock.bestDriver.num = localStock.tabResult[k].num;
					}
					if(DISPMODE == 1){
						if(localStock.tabResult[k].num < 10) printf("[Server 0%d] ", localStock.tabResult[k].num);
						else printf("[Server %d] ", localStock.tabResult[k].num);		
						if(tabRead[k].lnum < 10) printf(" | Lap 0%d: %6.2lf sec", tabRead[k].lnum, localStock.tabResult[k].timeLastLap);
						else printf(" | Lap %d: %6.2lf sec", localStock.tabResult[k].lnum, localStock.tabResult[k].timeLastLap);
						printf(" | Global: %10.2lf sec", localStock.tabResult[k].timeGlobal);
						printf(" | Retired? %3s", localStock.tabResult[k].retired ? "yes" : "no");
						printf(" | Pitstop? %3s", localStock.tabResult[k].pitstop ? "yes" : "no");	
						printf(" | NbFinished: %d", nbFinished);								
				  		printf("\n");
				  	}
				  	// write into the shared mem for monitor
				  	while(semGet(sem_DispSrv, 0) != 1);
					semDown(sem_DispSrv, 0);
					memcpy(&listStock->tabResult[k], &localStock.tabResult[k], sizeof(TResults));
					memcpy(&listStock->bestDriver, &localStock.bestDriver, sizeof(TBest));
					semUp(sem_DispSrv, 0);
					if((type == SIGGP) && (localStock.tabResult[k].lnum >= LAPGP)) finished = true;
					if((type != SIGGP) && ((localStock.tabResult[k].retired) || (localStock.tabResult[k].timeGlobal >= timeMax)) 
						&& (!tabFinished[k])){
						sendSig(SIGEND, sem_race, k);
						nbFinished++; 
						tabFinished[k] = true;
						if(nbFinished == maxCar) finished = true;
					}
				}
			}    
		} while(!finished);

		if(type == SIGGP){ // Terminate GP and send all last informations to monitor
			sendSig(SIGEND, sem_control, 0);
		}
    	show_notice("Server", "Waiting last drivers informations and end of run");
    	int s;
    	for(s=0; s<22; s++){
			do{
				while(semGet(sem_mutex, TMP1) != 1);
				semDown(sem_mutex, TMP1);
				memcpy(&tabRead[s].ready, &tabCar[s].ready, sizeof(bool));
				semUp(sem_mutex, TMP1);
			}while(tabRead[s].ready);
    	}
    	semReset(sem_mutex, TMP1);
    	for(s = 0; s<22; s++) semReset(sem_race, s);
    	sendSig(SIGEND, sem_control, 0);
    	TSharedStock tmpStock;
    	memcpy(&tmpStock, &localStock, sizeof(TSharedStock));
    	if(type == SIGGP) qsort(tmpStock.tabResult, 22, sizeof(TResults), (int (*)(const void*, const void*))cmpGP);

    	switch(type){
    		case SIGQU1 :
    					qsort(tmpStock.tabResult, 22, sizeof(TResults), (int (*)(const void*, const void*))cmpQual); 
    					for(i=21; i>14; i--){ 
    						tabOut[i].numPilot = tmpStock.tabResult[i].num; 
    						for(s=0; s<22; s++){
    							if(tabOut[i].numPilot == localStock.tabResult[s].num) tabOut[i].numCell = s;
    						}
    					}
    				  	break;	    					
    		case SIGQU2 :
    					qsort(tmpStock.tabResult, 22, sizeof(TResults), (int (*)(const void*, const void*))cmpQual);
    					for(i=14; i>7; i--) {
    						tabOut[i].numPilot = tmpStock.tabResult[i].num; 
    						for(s=0; s<22; s++){
    							if(tabOut[i].numPilot == localStock.tabResult[s].num) tabOut[i].numCell = s;
    						}
    					}
    				  	break;	
    		case SIGQU3 :
    					qsort(tmpStock.tabResult, 22, sizeof(TResults), (int (*)(const void*, const void*))cmpQual); 
    					for(i=7; i>=0; i--) {
    						tabOut[i].numPilot = tmpStock.tabResult[i].num; 
    						for(s=0; s<22; s++){
    							if(tabOut[i].numPilot == localStock.tabResult[s].num) tabOut[i].numCell = s;
    						}
    					}
    				  	break;	
    	}
		show_success("Server", "Race terminated!");
		if((stream = open(date_time, O_RDWR | O_CREAT, mode)) < 0){
			perror("Error while opening/creating message.\n");
		}
		if(type == SIGGP){
			TTabGP tabTmpGP;
			for(i=0; i<22; i++){
				tabTmpGP.results[i].pos = i+1;
				tabTmpGP.results[i].num = tmpStock.tabResult[i].num;
				tabTmpGP.results[i].lnum = tmpStock.tabResult[i].lnum + 1;
				tabTmpGP.results[i].teamName = tmpStock.tabResult[i].teamName;
				tabTmpGP.results[i].timeBestLap = tmpStock.tabResult[i].bestLapTime;
				tabTmpGP.results[i].timeGlobal = tmpStock.tabResult[i].timeGlobal;
				tabTmpGP.results[i].retired = tmpStock.tabResult[i].retired;
			}
			for(i=0;i<3;i++){
				tabTmpGP.bestSect[i].num;
				tabTmpGP.bestSect[i].teamName;
				tabTmpGP.bestSect[i].snum;
				tabTmpGP.bestSect[i].timeBestSect;
			}
			tabTmpGP.bestLap.num;
			tabTmpGP.bestLap.teamName;
			tabTmpGP.bestLap.lnum;
			tabTmpGP.bestLap.timeBestLap;
			write(stream,&tabTmpGP,sizeof(TTabGP)); 
		}
		else{
			TTabQT tabTmpQT; 
			for(i=0; i<22; i++){
				tabTmpQT.results[i].pos = i+1;
				tabTmpQT.results[i].num = tmpStock.tabResult[i].num;
				tabTmpQT.results[i].teamName = tmpStock.tabResult[i].teamName;
				tabTmpQT.results[i].timeBestLap = tmpStock.tabResult[i].bestLapTime;
				tabTmpQT.results[i].retired = tmpStock.tabResult[i].retired;
			}
			for(i=0;i<3;i++){
				tabTmpQT.bestSect[i].num  ;
				tabTmpQT.bestSect[i].teamName  ;
				tabTmpQT.bestSect[i].snum  ;
				tabTmpQT.bestSect[i].timeBestSect  ;
			}
			write(stream,&tabTmpQT,sizeof(TTabQT));
		}
		close(stream);
	}while(!checkSig(SIGEXIT, sem_control, 0));
	eop:
		shmdt(&shm_race);
		shmdt(&shm_DispSrv);
		if(semGet(sem_mutex, 0) != 1) semReset(sem_mutex, 0);
		if(semGet(sem_type, 0) != 1) semReset(sem_type, 0);
		if(semGet(sem_control, 0) != 1) semReset(sem_control, 0);
		if(semGet(sem_control, 1) != 1) semReset(sem_control, 1);
		semctl(sem_mutex, TMP1, IPC_RMID, NULL);
		semctl(sem_type, 0, IPC_RMID, NULL);
		semctl(sem_control, 0, IPC_RMID, NULL);
		semctl(sem_control, 1, IPC_RMID, NULL);
		int t;
		for(t = 0; t < 22; t++) {
			if(semGet(sem_race, t) != 1) semReset(sem_race, t);
			semctl(sem_race, t, IPC_RMID, NULL);
		}
		shmctl(shm_race, IPC_RMID, NULL);
		shmctl(shm_DispSrv, IPC_RMID, NULL);
		if(semGet(sem_mutex, 0) != 1) semReset(sem_mutex, 0);
		semctl(sem_DispSrv, 0, IPC_RMID, NULL);
		return;
}

// Sector table as parameter
// Calculate & return lap time 
double lapTime(TSect *tabSect){
	double tmp = tabSect[0].stime + tabSect[1].stime + tabSect[2].stime;
    return tmp;
}