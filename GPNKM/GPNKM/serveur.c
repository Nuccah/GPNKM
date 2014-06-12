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

    
	show_success("Server", "Initialisation complete");
	// END INIT SECTION
	do{
		int i = 0, j;
		TTabCar tabRead[22];

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
		
		// Wait for drivers ready
		for(i = 0; i < 22; i++){ 
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
			}while(!tabRead[i].ready);
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
		sendSig(SIGSTART, sem_control, 0); 
		double timeMax = 0.0;
		switch(type){
			case SIGTR1: timeMax = 5400.0;
					break;
			case SIGTR2: timeMax = 5400.0;
					break;
			case SIGTR3: timeMax = 3600.0;
					break;
			case SIGQU1: timeMax = 1100.0;
					break;
			case SIGQU2: timeMax = 600.0;
					break;
			case SIGQU3: timeMax = 800.0;
					break;
		}
		semReset(sem_type, 0);
		bool finished = false;
		bool tabFinished[22];
		for(i = 0; i < 22; i++) tabFinished[i] = false;
		int currentLap = 0;
		int tmpLap = 0, tmpSec = 0, k, nbFinished = 0;
		do {
			sleep(0.2);
			for(k = 0; k < 22; k++){
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
								if(localStock.tabResult[k].snum == 2) 
									localStock.tabResult[k].timeLastLap = lapTime(tabRead[k].lapTimes[tmpLap].tabSect);
								localStock.tabResult[k].timeGlobal += tabRead[k].lapTimes[tmpLap].tabSect[j].stime;
							}
						}
					}
					else
					{
						for(i=tmpLap; i <=tabRead[k].lnum; i++){
							if(i == tmpLap){
								for(j=tmpSec; j<=2; j++){
									if(localStock.tabResult[k].snum == 2) 
										localStock.tabResult[k].timeLastLap = lapTime(tabRead[k].lapTimes[i].tabSect);
									localStock.tabResult[k].timeGlobal += tabRead[k].lapTimes[i].tabSect[j].stime;
								}
							}
							else if(i == tabRead[k].lnum){
								for(j=0; j<=tabRead[k].snum; j++){
									if(localStock.tabResult[k].snum == 2) 
										localStock.tabResult[k].timeLastLap = lapTime(tabRead[k].lapTimes[i].tabSect);
									localStock.tabResult[k].timeGlobal += tabRead[k].lapTimes[i].tabSect[j].stime;
								}
							}
							else{
								for(j=0; j<=2; j++){
									if(localStock.tabResult[k].snum == 2) 
										localStock.tabResult[k].timeLastLap = lapTime(tabRead[k].lapTimes[i].tabSect);
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
					semUp(sem_DispSrv, 0);
					if((type == SIGGP) && (localStock.tabResult[k].lnum >= LAPGP)) goto end;
					if((type != SIGGP) && (localStock.tabResult[k].timeGlobal >= timeMax) && (!tabFinished[k])){
						sendSig(SIGEND, sem_race, k);
						nbFinished++; 
						tabFinished[k] = true;
						if(nbFinished == 22) finished = true;
					}
			}    
		} while(!finished);
		goto next;
		end: // Terminate GP and send all last informations to monitor
			sendSig(SIGEND, sem_control, 0);
	    next:
	    	//semReset(sem_mutex, TMP1);
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
	    	show_success("Server", "Race terminated!");
	}while(!checkSig(SIGEXIT, sem_control, 0));
	eop:
		shmdt(&shm_race);
		shmdt(&shm_DispSrv);
		semctl(sem_mutex, TMP1, IPC_RMID, NULL);
		semctl(sem_type, 0, IPC_RMID, NULL);
		semctl(sem_control, 0, IPC_RMID, NULL);
		semctl(sem_control, 1, IPC_RMID, NULL);
		int t;
		for(t = 0; t < 22; t++) semctl(sem_race, t, IPC_RMID, NULL);
		shmctl(shm_race, IPC_RMID, NULL);
		shmctl(shm_DispSrv, IPC_RMID, NULL);
		semctl(sem_DispSrv, 0, IPC_RMID, NULL);
		return;
}

// Sector table as parameter
// Calculate & return lap time 
double lapTime(TSect *tabSect){
	double tmp = tabSect[0].stime + tabSect[1].stime + tabSect[2].stime;
    return tmp;
}

// Executed when alarm() is at 0
void endRace(int sig){
    signal(SIGALRM, SIG_IGN);
    printf("\n-------------------- Alarm ended ------------------\n\n");
	key_t sem_control_key = ftok(PATH, CONTROL);
	int sem_control = semget(sem_control_key, 2, IPC_CREAT | PERMS);
	sendSig(SIGEND, sem_control, 0);
}

