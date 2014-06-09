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

    key_t sem_race_key = ftok(PATH, RACE);
    int sem_race = semget(sem_race_key, 22, IPC_CREAT | PERMS);
	
	key_t sem_switch_key = ftok(PATH, SWITCH);
	int sem_switch = semget(sem_switch_key, 22, IPC_CREAT | PERMS);

    key_t sem_DispSrv_key = ftok(PATH, STOCK);
    int sem_DispSrv = semget(sem_DispSrv_key, 2, IPC_CREAT | PERMS);

    key_t shm_DispSrv_key = ftok(PATH, STOCKSHM);
    int shm_DispSrv = shmget(shm_DispSrv_key, sizeof(TSharedStock), S_IWUSR);
	TSharedStock *listStock = (TSharedStock *) shmat(shm_DispSrv, NULL, 0);

	key_t shm_race_key = ftok(PATH, RACESHM);
	int shm_race = shmget(shm_race_key, 22*sizeof(TCar), S_IRUSR);
	TCar *tabCar = (TCar *) shmat(shm_race, NULL, 0);
    
	show_success("Server", "Initialisation complete");
	// END INIT SECTION
	do{
		int i = 0, j;
		TCar tabRead[22];

	    // Send weather to monitor and pilots
		sendSig(randomWeather(), sem_control, 1);
		// Wait race type from Monitor
		int type = 0;
		while((!((type >= SIGTR1) && (type <= SIGGP))) && (!checkSig(SIGEXIT, sem_control, 0))) type = getSig(sem_type, 0);
		if(checkSig(SIGEXIT, sem_control, 0)) goto eop;
		printf("Server received: %d\n", type);
		TSharedStock localStock;
		localStock.bestDriver.time = 0;

		semDown(sem_DispSrv, SRV_WRITE);
		for(j=0; j < 22; j++){
			listStock->tabResult[j].timeGlobal = 0.0;
			listStock->tabResult[j].timeLastLap = 0.0;
			listStock->tabResult[j].snum = 0;
			listStock->tabResult[j].lnum = 0;
			listStock->tabResult[j].retired = false;
			listStock->tabResult[j].pitstop = false;
		}
		semUp(sem_DispSrv, SRV_WRITE);
		semSwitch(sem_DispSrv, SRV_SWITCH);
		
		// Wait for drivers ready
		for(i = 0; i < 22; i++){ 
			do{
				int sswitch, sdrap;
				do{
					sswitch = semGet(sem_switch, i);
					sdrap = semGet(sem_race, i);
					memcpy(&tabRead[i], &tabCar[i], sizeof(TCar));
				} while((sdrap != 1) && (semGet(sem_race, i) != 1) && (sswitch != semGet(sem_switch, i)));
				localStock.tabResult[i].teamName = tabRead[i].teamName;
				localStock.tabResult[i].num = tabRead[i].num;
				localStock.tabResult[i].timeGlobal = 0.0;
				localStock.tabResult[i].timeLastLap = 0.0;
				localStock.tabResult[i].lnum = 0;
				localStock.tabResult[i].snum = 0;
			}while(!tabRead[i].ready);
		}
		semDown(sem_DispSrv, SRV_WRITE);
		memcpy(listStock, &localStock, sizeof(TSharedStock)); // Put stock content into shared memory
		semUp(sem_DispSrv, SRV_WRITE);
		semSwitch(sem_DispSrv, SRV_SWITCH);
		
		show_notice("Server", "All drivers are ready!");
		
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
		int tab_old[22];
		for(i=0; i<22;i++) tab_old[i] = 0;
		int tab_new[22];
		do {
			if((type != SIGGP) && checkSig(SIGEND, sem_control, 0)) finished = true;
			if((type == SIGGP) && (currentLap >= LAPGP)) goto end;
			else{
				for(k = 0; k < 22; k++){
					tab_new[k] = semGet(sem_switch, k);
					if(tab_new[k] != tab_old[k]){
						tab_old[k] = tab_new[k];
						// Read in shared table
						tmpLap = localStock.tabResult[k].lnum;
						tmpSec = localStock.tabResult[k].snum;

						int sswitch, sdrap;
						do{
							sdrap = semGet(sem_race, k);
							sswitch = semGet(sem_switch, k);
							memcpy(&tabRead[k].lnum, &tabCar[k].lnum, sizeof(int));
							memcpy(&tabRead[k].snum, &tabCar[k].snum, sizeof(int));
							for(i=tmpLap; i <= tabRead[k].lnum; i++){
								memcpy(&tabRead[k].lapTimes[i], &tabCar[k].lapTimes[i], sizeof(TLap));
							}
							memcpy(&tabRead[k].retired, &tabCar[k].retired, sizeof(bool));
							memcpy(&tabRead[k].pitstop, &tabCar[k].pitstop, sizeof(bool));
						} while((sdrap != 1) && (semGet(sem_race, k) != 1) && (sswitch != semGet(sem_switch, k)));
						
						localStock.tabResult[k].lnum = tabRead[k].lnum;
						localStock.tabResult[k].snum = tabRead[k].snum;
						
						for(i=tmpLap; i <= tabRead[k].lnum; i++){
							if(i == tmpLap) {
								for(j=tmpSec; j <= tabRead[k].snum; j++){
									if(localStock.tabResult[k].snum == 2) 
										localStock.tabResult[k].timeLastLap = lapTime(tabRead[k].lapTimes[tabRead[k].lnum].tabSect);
									localStock.tabResult[k].timeGlobal += tabRead[k].lapTimes[tabRead[k].lnum].tabSect[tabRead[k].snum].stime;
								}
							 }
							 else{
								for(j=0; j <= tabRead[k].snum; j++){
									if(localStock.tabResult[k].snum == 2) 
										localStock.tabResult[k].timeLastLap = lapTime(tabRead[k].lapTimes[tabRead[k].lnum].tabSect);
									localStock.tabResult[k].timeGlobal += tabRead[k].lapTimes[tabRead[k].lnum].tabSect[tabRead[k].snum].stime;
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
							else printf(" | Lap %d: %6.2lf sec", tabRead[k].lnum, localStock.tabResult[k].timeLastLap);
							printf(" | Global: %10.2lf sec", localStock.tabResult[k].timeGlobal);
							printf(" | Retired? %3s", localStock.tabResult[k].retired ? "yes" : "no");
							printf(" | Pitstop? %3s", localStock.tabResult[k].pitstop ? "yes" : "no");									
					  		printf("\n");
					  	}
					  	// write into the shared mem for monitor
						semDown(sem_DispSrv, SRV_WRITE);
						memcpy(&listStock->tabResult[k].retired, &localStock.tabResult[k].retired, sizeof(bool));
						memcpy(&listStock->tabResult[k].pitstop, &localStock.tabResult[k].pitstop, sizeof(bool));
						memcpy(&listStock->tabResult[k].timeLastLap, &localStock.tabResult[k].timeLastLap, sizeof(double));
						memcpy(&listStock->tabResult[k].timeGlobal, &localStock.tabResult[k].timeGlobal, sizeof(double));
						memcpy(&listStock->tabResult[k].lnum, &localStock.tabResult[k].lnum, sizeof(int));
						memcpy(&listStock->tabResult[k].snum, &localStock.tabResult[k].snum, sizeof(int));
						semUp(sem_DispSrv, SRV_WRITE);
						semSwitch(sem_DispSrv, SRV_SWITCH);
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
	    	int s;
	    	for(s=0;s<22;s++) semReset(sem_race, s);
	}while(!checkSig(SIGEXIT, sem_control, 0));
	eop:
		shmdt(&shm_race);
		shmdt(&shm_DispSrv);
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
	int sem_control = semget(sem_control_key, 1, IPC_CREAT | PERMS);
	sendSig(SIGEND, sem_control, 0);
}

