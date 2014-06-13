#include "afficheur.h"

void scoreMonitor(int sem_control, int type, int level, char *date_time){
	// INIT SECTION
	key_t sem_type_key = ftok(PATH, TYPE);
	int sem_type = semget(sem_type_key, 1, IPC_CREAT | PERMS);
	key_t sem_DispSrv_key = ftok(PATH, STOCK);
	int sem_DispSrv = semget(sem_DispSrv_key, 1, IPC_CREAT | PERMS);
	key_t shm_DispSrv_key = ftok(PATH, STOCKSHM);
	int shm_DispSrv = shmget(shm_DispSrv_key, sizeof(TSharedStock), S_IRUSR);

	TSharedStock *listStock = (TSharedStock *) shmat(shm_DispSrv, NULL, 0); 
	waitSig(SIGSTART, sem_control, 0);
	if(level == 6){
		show_notice("Monitor", "Grand Prix will begin in 3!!!");
		sleep(1); system("clear");
		show_notice("Monitor", "Grand Prix will begin in 2!!!");
		sleep(1); system("clear");
		show_notice("Monitor", "Grand Prix will begin in 1!!!");
	}
	else if(level < 3) show_notice("Monitor", "Trial Runs will begin in 1!!!");
	else show_notice("Monitor", "Qualifier Runs will begin in 1!!!");
	sleep(1);
	bool finished = false;
	int k, i;
	TSharedStock localStock;
	for(i = 0; i < 22; i++){
		localStock.tabResult[i].timeGlobal = 0.0;
		localStock.tabResult[i].timeLastLap = 0.0;
		localStock.tabResult[i].lnum = 0;
		localStock.tabResult[i].snum = 0;
	}
	do{
		if(checkSig(SIGEND, sem_control, 0)) finished = true;
		else{
			semDown(sem_DispSrv, 0);
			memcpy(&localStock, listStock, sizeof(TSharedStock));
			semUp(sem_DispSrv, 0);
			if(DISPMODE == 0){
				if(type == SIGGP) qsort(localStock.tabResult, 22, sizeof(TResults), (int (*)(const void*, const void*))cmpGP);
				else qsort(localStock.tabResult, 22, sizeof(TResults), (int (*)(const void*, const void*))cmpQual);
				int i;
				system("clear");
				if(type == SIGGP){
					printf(" Pos | Driver |        Team Name        | Global Time | Lap | Sect 1 | Sect 2 | Sect 3 | Best Lap | Retired | Pitstop \n");
					for(i = 0; i < 22; i++){
						printf(" %3d | %6d | %23s | %11.2lf | %3d | %6.2lf | %6.2lf | %6.2lf | %8.2lf | %7s | %7s \n", i+1, localStock.tabResult[i].num, 
									localStock.tabResult[i].teamName, localStock.tabResult[i].timeGlobal, localStock.tabResult[i].lnum,  
									localStock.tabResult[i].tabSect[0].stime, localStock.tabResult[i].tabSect[1].stime, localStock.tabResult[i].tabSect[2].stime, 
									localStock.tabResult[i].bestLapTime, localStock.tabResult[i].retired ? "yes" : "no", 
									localStock.tabResult[i].pitstop ? "yes" : "no");
					}
					printf("\n");
					printf("Best Lap\n");
					printf(" Driver |        Team Name        | Lap | Lap Time \n");
					printf(" %6d | %23s | %3d | %8.2lf \n", localStock.bestDriver.num, localStock.bestDriver.teamName, 
							localStock.bestDriver.lnum, localStock.bestDriver.time);
					printf("\n");
					printf("\n");
					printf("Best Sector");
					printf(" Driver |        Team Name        | Sector 1 | Sector 2 | Sector 3 \n");
					for(i=0;i<3;i++){
						if(i==0){
							printf(" %6d | %23s | %6.2lf | ------ | ------ \n", 
								localStock.bestSector[i].num, localStock.bestSector[i].teamName, 
								localStock.bestSector[i].time);
						}
						if(i==1){
							printf(" %6d | %23s | ------ | %6.2lf | ------ \n", 
								localStock.bestSector[i].num, localStock.bestSector[i].teamName, 
								localStock.bestSector[i].time);
						}
						if(i==2){
							printf(" %6d | %23s | ------ | ------ | %6.2lf \n", 
								localStock.bestSector[i].num, localStock.bestSector[i].teamName, 
								localStock.bestSector[i].time);
						}
					}
					printf("\n");		
				} 
				else{
					printf(" Pos | Driver |        Team Name        | Lap | Sect 1 | Sect 2 | Sect 3 | Best Lap | Retired | Pitstop \n");
					for(i = 0; i < 22; i++){
						printf(" %3d | %6d | %23s | %3d | %6.2lf | %6.2lf | %6.2lf | %8.2lf | %7s | %7s \n", i+1, localStock.tabResult[i].num, 
									localStock.tabResult[i].teamName, localStock.tabResult[i].lnum,  
									localStock.tabResult[i].tabSect[0].stime, localStock.tabResult[i].tabSect[1].stime, localStock.tabResult[i].tabSect[2].stime, 
									localStock.tabResult[i].bestLapTime, localStock.tabResult[i].retired ? "yes" : "no",
									localStock.tabResult[i].pitstop ? "yes" : "no");
					}
					printf("\n");
					printf("Best Sector\n");
					printf(" Driver |        Team Name        | Sector 1 | Sector 2 | Sector 3 \n");
					for(i=0;i<3;i++){
						if(i==0){
							printf(" %6d | %23s | %6.2lf | ------ | ------ \n", 
								localStock.bestSector[i].num, localStock.bestSector[i].teamName, 
								localStock.bestSector[i].time);
						}
						if(i==1){
							printf(" %6d | %23s | ------ | %6.2lf | ------ \n", 
								localStock.bestSector[i].num, localStock.bestSector[i].teamName, 
								localStock.bestSector[i].time);
						}
						if(i==2){
							printf(" %6d | %23s | ------ | ------ | %6.2lf \n", 
								localStock.bestSector[i].num, localStock.bestSector[i].teamName, 
								localStock.bestSector[i].time);
						}
					}
					printf("\n");
				} 
				printf("\n");					
			}
			sleep(1);
		} 
	}while(!finished);
	shmdt(&shm_DispSrv);
	semReset(sem_type, 0);
	fflush(stdin);
	printf("Press 0 to return to main menu...\n");
	while(getchar() != '0') fflush(stdin);
	level++;
	showMainMenu(level, date_time);
}

void showTRMenu(int sem_control, int sem_type, int level, char *date_time){
	system ( "clear" );
	fflush(stdin);
	printf("\033[36m");
	printf ("Trial Runs!\n");
	printf ("-------------------------------------\n\n");
	if (level == 0) printf ("1 : Begin Trial Run 1\n");
	if (level == 1) printf ("2 : Begin Trial Run 2\n");
	if (level == 2) printf ("3 : Begin Trial Run 3\n");
	printf ("0 : Back\n");
	printf ("-------------------------------------\033[0m \n\n");
	switch(getchar())
	{
		case '1' :
				fflush(stdin); 
				if (level == 0){
					show_notice("Monitor", "Trial 1 is going to begin");
				    sendSig(SIGTR1, sem_type, 0);
				    scoreMonitor(sem_control, SIGTR1, level, date_time);
				}
				break;
		case '2' : 
				fflush(stdin);
				if (level == 1){
					show_notice("Monitor", "Trial 2 is going to begin");
				    sendSig(SIGTR2, sem_type, 0);
				    scoreMonitor(sem_control, SIGTR2, level, date_time);
				}
				break;
		case '3' : 
				fflush(stdin);
				if (level == 2){
					show_notice("Monitor", "Trial 3 is going to begin");
				    sendSig(SIGTR3, sem_type, 0);
				    scoreMonitor(sem_control, SIGTR3, level, date_time);
				}
				break;
		case '0' : 
			fflush(stdin);
			showMainMenu(level, date_time); 
			break;
		default  : 
			fflush(stdin);
			showTRMenu(sem_control, sem_type, level, date_time); 
			break;
	}
}

void showQualifMenu(int sem_control, int sem_type, int level, char *date_time){
	system ( "clear" );
	fflush(stdin);
	printf("\033[36m");
	printf ("Welcome to the worldest famous GPNKM!\n");
	printf ("-------------------------------------\n\n");
	if (level == 3) printf ("1 : Begin Qualifier 1\n");
	if (level == 4) printf ("2 : Begin Qualifier 2\n");
	if (level == 5) printf ("3 : Begin Qualifier 3\n");
	printf ("0 : Back\n");
	printf ("-------------------------------------\033[0m \n\n");
	switch(getchar())
	{
		case '1' :
				if (level == 3){
					show_notice("Monitor", "Qualification 1 is going to begin");
				   	sendSig(SIGQU1, sem_type, 0);
				   	scoreMonitor(sem_control, SIGQU1, level, date_time);
				}
				break;	
		case '2' : 
				if (level == 4){
					show_notice("Monitor", "Qualification 1 is going to begin");
					sendSig(SIGQU2, sem_type, 0);
					scoreMonitor(sem_control, SIGQU2, level, date_time);
				}
				break;
		case '3' : 
				if (level == 5){
					show_notice("Monitor", "Qualification 1 is going to begin");
					sendSig(SIGQU3, sem_type, 0);
					scoreMonitor(sem_control, SIGQU3, level, date_time);
				}
				break;
		case '0' : 
			showMainMenu(level, date_time); 
			break;
		default  :
			showQualifMenu(sem_control, sem_type, level, date_time); 
			break;
	} 
}

void showMainMenu(int level, char *date_time){
	fflush(stdin);
	key_t sem_type_key = ftok(PATH, TYPE);
	int sem_type = semget(sem_type_key, 1, IPC_CREAT | PERMS);
	
	key_t sem_control_key = ftok(PATH, CONTROL);
	int sem_control = semget(sem_control_key, 2, IPC_CREAT | PERMS);
	int weather = 1;
	while(!((weather >= SIGDRY) && (weather <= SIGRAIN))){
		usleep(2000);
		weather = getSig(sem_control, 1);
	} 	

	system ( "clear" );
	printf("\033[36m");
	printf ("Welcome to the worldest famous GPNKM!\n");
	weatherMsg(weather);
	printf ("-------------------------------------\n\n");
	if (level < 3) printf ("1 : Begin Test Runs\n");
	if (level > 2 && level < 6) printf ("2 : Begin Qualifiers\n");
	if (level == 6) printf ("3 : Begin Grand Prix\n");
	if (level > 2)printf ("4 : Show Results of Grand Prix\n");
	printf ("0 : Exit\n");
	printf ("-------------------------------------\033[0m \n\n");
	switch(getchar())
	{
		case '1' : 
				if(level < 3){
					showTRMenu(sem_control, sem_type, level, date_time); 
				}
				break;
		case '2' : 
				if(level > 2 && level < 6){
					showQualifMenu(sem_control, sem_type, level, date_time);
				}	
				break; 
		case '3' : 
				if(level == 6){
					show_notice("Monitor", "Grand Prix is going to begin");
			   		sendSig(SIGGP, sem_type, 0);
			   		scoreMonitor(sem_control, SIGGP, level, date_time);
				}  
				break; 	
		case '4' :  
					if(level > 2) afficheResultats(date_time, level); 
				break;
		case '0' : endOfProgram(sem_control, sem_type); break;
		default  : 
			showMainMenu(level, date_time); 
			break;
	}
}

void endOfProgram(int sem_control, int sem_type){
	key_t sem_DispSrv_key = ftok(PATH, STOCK);
	int sem_DispSrv = semget(sem_DispSrv_key, 1, IPC_CREAT | PERMS);
	sendSig(SIGEXIT, sem_control, 0);
	sleep(1);
	if(semGet(sem_type, 0) != 1) semReset(sem_type, 0);
	semctl(sem_type, 0, IPC_RMID, NULL);
	if(semGet(sem_DispSrv, 0) != 1) semReset(sem_DispSrv, 0);
	semctl(sem_DispSrv, 0, IPC_RMID, NULL);
	key_t shm_DispSrv_key = ftok(PATH, STOCKSHM);
	int shm_DispSrv = shmget(shm_DispSrv_key, sizeof(TSharedStock), S_IRUSR | S_IWUSR);
	shmctl(shm_DispSrv, IPC_RMID, NULL);
	show_success("Monitor", "All processes closed successfully\nThanks for your presence at GPNKM championship");
	if(semGet(sem_control, 0) != 1) semReset(sem_control, 0);
	semctl(sem_control, 0, IPC_RMID, NULL);
}

void weatherMsg(int number){
	switch( number ) {
    	case SIGRAIN: printf("It's a rainy day at the normally dry GPNKM track, drivers should prepare for a tough weekend!\n"); break;
    	case SIGWET: printf("The weather has been off and on, drivers need to be ready for a wet circuit!\n"); break;
    	case SIGDRY: printf("It's a beautiful day at the GPNKM circuit, it is time to DRIVE!\n"); break;
	}
	return;
}

void afficheResultats(char *date_time, int level){
	int stream = 0, i, j, nbReads;
	TTabGP tabResultsGP;
	fflush(stdout); getchar();
	if((stream = open(date_time, O_RDWR | O_CREAT)) < 0)
	{
		perror("Error while opening/creating message.\n");
	}
	lseek(stream, 0, SEEK_SET);
	if (level < 6){
		nbReads = 3;
	} 
	else if(level > 5 && level < 7){
		nbReads = 6;
	}
	TTabQT tabResultsQT[nbReads]; 
	for(i=0; i<nbReads; i++){
		read(stream,&tabResultsQT[i],sizeof(TTabQT));
	}
	if(level == 7){
		TTabGP tabResultsGP;
		read(stream,&tabResultsGP,sizeof(TTabGP));
	} 
	close(stream);
	system ( "clear" );
	printf("\033[36m");
	printf ("World Famous GPNKM Race!\n");
	printf ("-------------------------------------\n\n");
	if (level > 2) printf ("1 : Show Trial Results\n");
	if (level > 5) printf ("2 : Show Qualifier Results\n");
	if (level == 7) printf ("3 : Show Grand Prix Results\n");
	printf ("0 : Back\n");
	printf ("-------------------------------------\033[0m \n\n");
	switch(getchar())
	{
		case '1' : 
				if(level > 2){
					afficheQT(tabResultsQT[0], tabResultsQT[1], tabResultsQT[2], date_time, level); 
					break;
				}
		case '2' : 
				if(level > 5){
					afficheQT(tabResultsQT[3], tabResultsQT[4], tabResultsQT[5], date_time, level);
					break;
				}	 
		case '3' : 
				if(level == 7){
			   		afficheGrandPrix(tabResultsGP, date_time, level);
			   		break;
				}   	
		default  : 
				showMainMenu(level, date_time); 
				break;
	}
}

void afficheQT(TTabQT Results1, TTabQT Results2, TTabQT Results3, char *date_time, int level){
	fflush(stdin);
	int i;
	system ( "clear" );
	printf("\033[36m");
	printf ("World Famous GPNKM Race!\n");
	printf ("Result of Run 1!\n");
	printf ("-------------------------------------\n\n");
	printf(" Pos | Driver |        Team Name        | Best Lap | Retired \n");
	for(i=0; i<22; i++) printf(" %3d | %6d | %23s | %8.2lf | %7s \n", Results1.results[i].pos, Results1.results[i].num, Results1.results[i].teamName,
								Results1.results[i].timeBestLap, Results1.results[i].retired ? "yes" : "no");
	printf ("-------------------------------------\n\n");
	fflush(stdin);
	printf("Press any key to show results of run 2\n");
	getchar();
	printf ("World Famous GPNKM Race!\n");
	printf ("Result of Run 2!\n");
	printf ("-------------------------------------\n\n");
	printf(" Pos | Driver |        Team Name        | Best Lap | Retired \n");
	for(i=0; i<22; i++) printf(" %3d | %6d | %23s | %8.2lf | %7s \n", Results2.results[i].pos, Results2.results[i].num, Results2.results[i].teamName,
								Results2.results[i].timeBestLap, Results2.results[i].retired ? "yes" : "no");
	printf ("-------------------------------------\n\n");
	fflush(stdin);
	printf("Press any key to show results of run 3\n");
	getchar();
	printf ("World Famous GPNKM Race!\n");
	printf ("Result of Run 3!\n");
	printf ("-------------------------------------\n\n");
	printf(" Pos | Driver |        Team Name        | Best Lap | Retired \n");
	for(i=0; i<22; i++) printf(" %3d | %6d | %23s | %8.2lf | %7s \n", Results3.results[i].pos, Results3.results[i].num, Results3.results[i].teamName,
								Results3.results[i].timeBestLap, Results3.results[i].retired ? "yes" : "no");
	printf ("-------------------------------------\n\n");
	fflush(stdin);
	printf("Press any key to go back\n");
	getchar();
	afficheResultats(date_time, level);
}

void afficheGrandPrix(TTabGP ResultsGP, char *date_time, int level){
	fflush(stdin);
	int i;
	system ( "clear" );
	printf("\033[36m");
	printf ("World Famous GPNKM Race!\n");
	printf ("Final Results of the Grand Prix!\n");
	printf ("-------------------------------------\n\n");
	printf(" Pos | Driver |        Team Name        | Laps | Global Time | Best Lap | Retired \n");
	for(i=0; i<22; i++) printf(" %3d | %6d | %23s | %4d | %8.2lf | %8.2lf | %7s \n", ResultsGP.results[i].pos, ResultsGP.results[i].num, 
									ResultsGP.results[i].teamName, ResultsGP.results[i].lnum, ResultsGP.results[i].timeGlobal, 
									ResultsGP.results[i].timeBestLap, ResultsGP.results[i].retired ? "yes" : "no");
	printf ("-------------------------------------\n\n");
	printf ("Best Lap Result of the Grand Prix!\n");
	printf ("-------------------------------------\n\n");
	printf(" Driver |        Team Name        | Lap Number | Best Lap \n");
	printf(" %3d | %23s | %4d | %8.2lf \n", ResultsGP.results[i].num, ResultsGP.results[i].teamName, 
											ResultsGP.results[i].lnum, ResultsGP.results[i].timeBestLap);
	printf ("-------------------------------------\n\n");
	printf("Press any key to go back\n");
	getchar();
	afficheResultats(date_time, level);
}