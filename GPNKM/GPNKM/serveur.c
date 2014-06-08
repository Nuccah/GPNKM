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
    int sem_control = semget(sem_control_key, 1, IPC_CREAT | PERMS);

    key_t sem_race_key = ftok(PATH, RACE);
    int sem_race = semget(sem_race_key, 22, IPC_CREAT | PERMS);

    key_t sem_modif_key = ftok(PATH, MODIF);
    int sem_modif = semget(sem_modif_key, 22, IPC_CREAT | PERMS);

    key_t sem_modifa_key = ftok(PATH, MODIFA); // Sema Key generated
	int sem_modifa = semget(sem_modifa_key, 1, IPC_CREAT | PERMS); // sema ID containing 22 physical sema!!
	semDown(sem_modifa,0);

    key_t sem_DispSrv_key = ftok(PATH, STOCK);
    int sem_DispSrv = semget(sem_DispSrv_key, 2, IPC_CREAT | PERMS);

    key_t shm_DispSrv_key = ftok(PATH, STOCKSHM);
    int shm_DispSrv = shmget(shm_DispSrv_key, sizeof(TSharedStock), S_IWUSR);
	TSharedStock *listStock = (TSharedStock *) shmat(shm_DispSrv, NULL, 0);

	key_t shm_race_key = ftok(PATH, RACESHM);
	int shm_race = shmget(shm_race_key, 22*sizeof(TCar), S_IRUSR);
	TCar *tabCar = (TCar *) shmat(shm_race, NULL, 0);
    
    int i; int j;
	TCar tabRead[22];
	show_success("Server", "Initialisation complete");
	// END INIT SECTION
	do{
		

	    // Send weather to monitor and pilots
		sendSig(randomWeather(), sem_control, 1);
		// Wait race type from Monitor
		int type = 0;
		while(!((type >= SIGTR1) && (type <= SIGGP))) type = getSig(sem_type, 0);
		printf("Server received: %d\n", type);
		TSharedStock localStock;
		localStock.bestDriver.time = 0;
		memset(&localStock, 0, sizeof(TSharedStock));
		memset(listStock, 0, sizeof(TSharedStock));
		// Wait for drivers ready
		for(i = 0; i < 22; i++){
			while(!isShMemReadable(sem_race, i));
			memcpy(&tabRead[i], &tabCar[i], sizeof(TCar));
			localStock.tabResult[i].teamName = tabRead[i].teamName;
			localStock.tabResult[i].num = tabRead[i].num;
			localStock.tabResult[i].timeGlobal = 0;
			localStock.tabResult[i].timeLastLap = 0;
			semDown(sem_modif, i);
		}
		semDown(sem_DispSrv, SRV_WRITE);
		memcpy(listStock, &localStock, sizeof(TSharedStock)); // Put stock content into shared memory
		semUp(sem_DispSrv, SRV_WRITE);
		semReset(sem_modifa,0);
		sleep(1);
		// Init signal handler if race type based on time
		if(type != SIGGP) signal(SIGALRM, endRace);
		semReset(sem_control, 0);
		semReset(sem_control, 1);
		// Send start signal
		sendSig(SIGSTART, sem_control, 0); 
		switch(type){
			case SIGTR1: alarm(54);
					break;
		case SIGTR2: alarm(54);
					break;
			case SIGTR3: alarm(36);
					break;
			case SIGQU1: alarm(11);
					break;
			case SIGQU2: alarm(6);
					break;
			case SIGQU3: alarm(8);
					break;
		}
		semReset(sem_type, 0);
		bool finished = false;
		int currentLap = 0;
		int tmpLap = 0, tmpSec = 0, k;
		sleep(2);
		do {
			if((type != SIGGP) && checkSig(SIGEND, sem_control, 0)) finished = true;
			if((type == SIGGP) && (currentLap >= LAPGP)) goto end;
			else{
					for(k = 0; k < 22; k++){
						if(isShMemReadable(sem_modif, k))
						{
							semDown(sem_modif, k);
							if(isShMemReadable(sem_race, k))
							{
								// Read in shared table
								tmpLap = localStock.tabResult[k].lnum;
								tmpSec = localStock.tabResult[k].snum;
								memcpy(&tabRead[k], &tabCar[k], sizeof(TCar));
								localStock.tabResult[k].lnum = tabRead[k].lnum;
								localStock.tabResult[k].snum = tabRead[k].snum;
								for(i=tmpLap; i<=tabRead[k].lnum; i++)
								{
									for(j=tmpSec; j<=tabRead[k].lapTimes[i].tabSect[j].stime; j++)
									{
										if(localStock.tabResult[k].snum == 2)
										{
											localStock.tabResult[k].timeLastLap = lapTime(tabRead[k].lapTimes[tabRead[k].lnum].tabSect);
										}
										localStock.tabResult[k].timeGlobal += tabRead[k].lapTimes[i].tabSect[j].stime;
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
								semReset(sem_modifa,0);
								 // write into the shared mem for monitor
							    qsort(localStock.tabResult, 22, sizeof(TResults), (int (*)(const void*, const void*))cmpfunct);
								 
							    while(!isShMemReadable(sem_DispSrv, DISP_READ));
								semDown(sem_DispSrv, SRV_WRITE);
								memcpy(listStock, &localStock, sizeof(TSharedStock)); // Put stock content into shared memory
								semUp(sem_DispSrv, SRV_WRITE);
							}
							else k--;
						}
					}
				if(type == SIGGP) currentLap++;
			}	    
		} while(!finished);
		goto next;
		end: // Terminate GP and send all last informations to monitor
			sendSig(SIGEND, sem_control, 0);
	    next:
	    	show_success("Server", "Race terminated!");
	    	sendSig(SIGEND, sem_control, 0);
	    	semReset(sem_type, 0);
	}while(!checkSig(SIGEXIT, sem_control, 0));
	shmdt(&shm_race);
	shmdt(&shm_DispSrv);
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
	int sem_control = semget(sem_control_key, 1, IPC_CREAT | PERMS);
	sendSig(SIGEND, sem_control, 0);
}

int cmpfunct(TResults *a, TResults *b){
	if(a->lnum == b->lnum)
	{
		if(a->snum ==  b->snum)
			if (a->timeGlobal <  b->timeGlobal) return -1;
			else if (a->timeGlobal >  b->timeGlobal) return 1;
			else return 0;
		else
		{
			if (a->snum < b->snum) return 1;
			else return -1;
		} 
			
	}
	else
		if (a->lnum < b->lnum) return 1;
		else return -1;
}
