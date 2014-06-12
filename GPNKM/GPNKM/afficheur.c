#include "afficheur.h"

void scoreMonitor(int sem_control, int type, int level){
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
			while(semGet(sem_DispSrv, 0) != 1);
			semDown(sem_DispSrv, 0);
			memcpy(&localStock, listStock, sizeof(TSharedStock));
			semUp(sem_DispSrv, 0);
			if(DISPMODE == 0){
				if(type == SIGGP) qsort(localStock.tabResult, 22, sizeof(TResults), (int (*)(const void*, const void*))cmpGP);
				else qsort(localStock.tabResult, 22, sizeof(TResults), (int (*)(const void*, const void*))cmpQual);
				int i;
				system("clear");
				for(i = 0; i < 22; i++){
					if((i+1) < 10) printf("[0%d] | ", i+1);
					else printf("[%d] | ", i+1);
					if(localStock.tabResult[i].num < 10) printf("[Driver 0%d] ", localStock.tabResult[i].num); 
					else printf("[Driver %d] ", localStock.tabResult[i].num);
					printf("lap: %3d | time: %10.2lf sec | best: %7.2lf sec | last: %7.2lf sec | ",
							localStock.tabResult[i].lnum, localStock.tabResult[i].timeGlobal,
							localStock.tabResult[i].bestLapTime, localStock.tabResult[i].timeLastLap);
					printf("Retired : %3s", localStock.tabResult[i].retired ? "yes" : "no");
					printf(" | Pitstop : %3s\n", localStock.tabResult[i].pitstop ? "yes" : "no");
				}					
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
	showMainMenu(level);
}

void showTRMenu(int sem_control, int sem_type, int level){
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
				if (level == 3){
					show_notice("Monitor", "Trial 1 is going to begin");
				    sendSig(SIGTR1, sem_type, 0);
				    scoreMonitor(sem_control, SIGTR1, level);
				    break;
				}
		case '2' : 
				fflush(stdin);
				if (level == 3){
					show_notice("Monitor", "Trial 2 is going to begin");
				    sendSig(SIGTR2, sem_type, 0);
				    scoreMonitor(sem_control, SIGTR2, level);
				    break;
				}
		case '3' : 
				fflush(stdin);
				if (level == 3){
					show_notice("Monitor", "Trial 3 is going to begin");
				    sendSig(SIGTR3, sem_type, 0);
				    scoreMonitor(sem_control, SIGTR3, level);
				    break;
				}
		case '0' : 
			fflush(stdin);
			showMainMenu(level); 
			break;
		default  : 
			fflush(stdin);
			showTRMenu(sem_control, sem_type, level); 
			break;
	}
}

void showQualifMenu(int sem_control, int sem_type, int level){
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
				   	scoreMonitor(sem_control, SIGQU1, level);
				  	break;
				}	
		case '2' : 
				if (level == 4){
					show_notice("Monitor", "Qualification 1 is going to begin");
					sendSig(SIGQU2, sem_type, 0);
					scoreMonitor(sem_control, SIGQU2, level);
					break;
				}
		case '3' : 
				if (level == 5){
					show_notice("Monitor", "Qualification 1 is going to begin");
					sendSig(SIGQU3, sem_type, 0);
					scoreMonitor(sem_control, SIGQU3, level);
					break;
				}
		case '0' : 
			showMainMenu(level); 
			break;
		default  :
			showQualifMenu(sem_control, sem_type, level); 
			break;
	} 
}

void showMainMenu(int level){
	fflush(stdin);
	key_t sem_type_key = ftok(PATH, TYPE);
	int sem_type = semget(sem_type_key, 1, IPC_CREAT | PERMS);
	
	key_t sem_control_key = ftok(PATH, CONTROL);
	int sem_control = semget(sem_control_key, 2, IPC_CREAT | PERMS);
	int weather = 1;
	while(!((weather >= SIGDRY) && (weather <= SIGRAIN))) weather = getSig(sem_control, 1);	

	system ( "clear" );
	printf("\033[36m");
	printf ("Welcome to the worldest famous GPNKM!\n");
	weatherMsg(weather);
	printf ("-------------------------------------\n\n");
	if (level < 3) printf ("1 : Begin Test Runs\n");
	if (level > 2 && level < 6) printf ("2 : Begin Qualifiers\n");
	if (level == 6) printf ("3 : Begin Grand Prix\n");
	printf ("4 : Show Results of Grand Prix\n");
	printf ("5 : Restart Grand Prix\n");
	printf ("0 : Exit\n");
	printf ("-------------------------------------\033[0m \n\n");
	switch(getchar())
	{
		case '1' : 
				if(level < 3){
					showTRMenu(sem_control, sem_type, level); 
					break;
				}
		case '2' : 
				if(level > 2 && level < 6){
					showQualifMenu(sem_control, sem_type, level);
					break;
				}	 
		case '3' : 
				if(level == 6){
					show_notice("Monitor", "Grand Prix is going to begin");
			   		sendSig(SIGGP, sem_type, 0);
			   		scoreMonitor(sem_control, SIGGP, level);
			   		break;
				}   	
		case '4' : printf("Show Results\n"); break;
		case '5' : printf("Restart Grand Prix\n"); break;
		case '0' : endOfProgram(sem_control, sem_type); break;
		default  : 
			showMainMenu(level); 
			break;
	}
}

void endOfProgram(int sem_control, int sem_type){
	key_t sem_DispSrv_key = ftok(PATH, STOCK);
	int sem_DispSrv = semget(sem_DispSrv_key, 1, IPC_CREAT | PERMS);
	sendSig(SIGEXIT, sem_control, 0);
	sleep(1);
	semctl(sem_type, 0, IPC_RMID, NULL);
	semctl(sem_DispSrv, 0, IPC_RMID, NULL);
	key_t shm_DispSrv_key = ftok(PATH, STOCKSHM);
	int shm_DispSrv = shmget(shm_DispSrv_key, sizeof(TSharedStock), S_IRUSR | S_IWUSR);
	shmctl(shm_DispSrv, IPC_RMID, NULL);
	show_success("Monitor", "All processes closed successfully\nThanks for your presence at GPNKM championship");
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

void afficheResultats(){
	/*
	int stream;
	time_t now;
	/* Time Function for filename definition 
	now = time(NULL);
	struct tm *time = localtime(&now);
	char date_time[30];
	strftime( date_time, sizeof(date_time), "%d%m%y_%H%M%S", time );
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH | O_TRUNC;
	if((stream = open(date_time, O_RDWR | O_CREAT, mode)) < 0)
{
	perror("Error while opening/creating message.\n");
}
TWriteQT tmp2;		
for(i=0; i<22; i++){
	read(stream,&tmp2,sizeof(TWriteQT));
	printf("%d | %d | %s | %0.2lf | %s\n", tmp2.pos, tmp2.num, tmp2.teamName, tmp2.timeBestLap, tmp2.retired ? "yes" : "no");
	printf("bloop\n");
}
close(stream);*/
}

void afficheTrials(){

}

void afficheQualif(){

}

void afficheGrandPrix(){

}