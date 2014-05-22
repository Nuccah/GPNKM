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

void server(int queue_id, int pfdSrvDrv, int pfdDrvSrv, TmsgbufAdr adr_msg){
    // INIT SECTION
    key_t sem_type_key = ftok(PATH, TYPE);
    int sem_type = semget(sem_type_key, 1, IPC_CREAT | PERMS);

    key_t sem_control_key = ftok(PATH, CONTROL);
    int sem_control = semget(sem_control_key, 1, IPC_CREAT | PERMS);

    key_t sem_race_key = ftok(PATH, RACE);
    int sem_race = semget(sem_race_key, 22, IPC_CREAT | PERMS);

    key_t sem_DispSrv_key = ftok(PATH, STOCK);
    int sem_DispSrv = semget(sem_DispSrv_key, 2, IPC_CREAT | PERMS);

    key_t shm_DispSrv_key = ftok(PATH, STOCKSHM);
    int shm_DispSrv = shmget(shm_DispSrv_key, sizeof(TSharedStock), S_IWUSR);
	TSharedStock *listStock = (TSharedStock *) shmat(shm_DispSrv, NULL, 0);

	key_t shm_race_key = ftok(PATH, RACESHM);
	int shm_race = shmget(shm_race_key, 22*sizeof(TCar), S_IRUSR);
	TCar *tabCar = (TCar *) shmat(shm_race, NULL, 0);
    // END INIT SECTION
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

	do{
		// Wait race type from Monitor
		int type = 0;
		while(!((type >= SIGTR1) && (type <= SIGGP))) type = getSig(sem_type, 0);
		printf("Server received: %d\n", type);

		TSharedStock localStock;
		localStock.bestDriver.time = 0;

		// Wait for drivers ready
		for(i = 0; i < 22; i++){
			while(!isShMemReadable(sem_race, i));
			tabRead[i] = tabCar[i];
			localStock.tabResult[i].teamName = tabRead[i].teamName;
			localStock.tabResult[i].num = tabRead[i].num;
			localStock.tabResult[i].timeGlobal = 0;
			localStock.tabResult[i].timeLastLap = 0;
		}
		sleep(1);
		// Init signal handler if race type based on time
		if(type != SIGGP) signal(SIGALRM, endRace);

		// Send start signal
		sendSig(SIGSTART, sem_control, 0); // THIS POINT BUUUUUUUGS !!!! WTF?
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
		bool finished = false;
		int currentLap = 0;
		do {
			if((type != SIGGP) && checkSig(SIGEND, sem_control, 0)) finished = true;
			else if((type == SIGGP) && (currentLap >= LAPGP)) goto end;
			else{
				int k;
				for(k = 0; k < 22; k++){
					if(isShMemReadable(sem_race, k))
					{
						// Read in shared table
						tabRead[k] = tabCar[k];
						localStock.tabResult[k].teamName = tabRead[k].teamName;
						localStock.tabResult[k].num = tabRead[k].num;
						localStock.tabResult[k].lnum = tabRead[k].lnum;

						// Calculate lap time
						localStock.tabResult[k].timeLastLap = lapTime(tabRead[k].lapTimes[tabRead[k].lnum].tabSect);
						localStock.tabResult[k].timeGlobal += localStock.tabResult[k].timeLastLap;

						localStock.tabResult[k].retired = tabRead[k].retired;
						localStock.tabResult[k].pitstop = tabRead[k].pitstop;
						if(localStock.bestDriver.time > localStock.tabResult[k].timeLastLap) // if best lap time is bigger than timeLastLap 
						{
							localStock.bestDriver.time = localStock.tabResult[k].timeLastLap;
							localStock.bestDriver.teamName = localStock.tabResult[k].teamName;
							localStock.bestDriver.num = localStock.tabResult[k].num;
						}
					    // write into the shared mem for monitor
					    while(!isShMemReadable(sem_DispSrv, DISP_READ));
						semDown(sem_DispSrv, SRV_WRITE);
						*listStock = localStock;
						semUp(sem_DispSrv, SRV_WRITE);
					} 
					else k--;
				}
				if(type == SIGGP) currentLap++;
			}	    
		} while(!finished);
		goto next;
		end: // Terminate GP and send all last informations to monitor
			sendSig(SIGEND, sem_control, 0);

	    next:
	    	show_success("Server", "Race terminated!");
	    	semReset(sem_control, 0);
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
