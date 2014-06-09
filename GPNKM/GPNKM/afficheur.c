#include "afficheur.h"

void scoreMonitor(int sem_control, int type){
	// INIT SECTION
	key_t sem_type_key = ftok(PATH, TYPE);
	int sem_type = semget(sem_type_key, 1, IPC_CREAT | PERMS);
	key_t sem_DispSrv_key = ftok(PATH, STOCK);
	int sem_DispSrv = semget(sem_DispSrv_key, 2, IPC_CREAT | PERMS);
	key_t shm_DispSrv_key = ftok(PATH, STOCKSHM);
	int shm_DispSrv = shmget(shm_DispSrv_key, sizeof(TSharedStock), S_IRUSR);

	TSharedStock *listStock = (TSharedStock *) shmat(shm_DispSrv, NULL, 0); 
	waitSig(SIGSTART, sem_control, 0);
	printf("Run begins!!!\n\n");
	bool finished = false;
	int new_switch, old_switch = 1, k, i;
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
			new_switch = semGet(sem_DispSrv, SRV_SWITCH);	
			if(new_switch != old_switch){
				old_switch = new_switch;
				int sswitch, sdrap;
				do{
					sdrap = semGet(sem_DispSrv, SRV_WRITE);
					sswitch = semGet(sem_DispSrv, SRV_SWITCH);
					for(k=0; k<22; k++){
						memcpy(&localStock.tabResult[k].retired, &listStock->tabResult[k].retired, sizeof(bool));
						memcpy(&localStock.tabResult[k].pitstop, &listStock->tabResult[k].pitstop, sizeof(bool));
						memcpy(&localStock.tabResult[k].timeLastLap, &listStock->tabResult[k].timeLastLap, sizeof(double));
						memcpy(&localStock.tabResult[k].timeGlobal, &listStock->tabResult[k].timeGlobal, sizeof(double));
						memcpy(&localStock.tabResult[k].lnum, &listStock->tabResult[k].lnum, sizeof(int));
						memcpy(&localStock.tabResult[k].snum, &listStock->tabResult[k].snum, sizeof(int));
						memcpy(&localStock.tabResult[k].teamName, &listStock->tabResult[k].teamName, sizeof(char *));
						memcpy(&localStock.tabResult[k].num, &listStock->tabResult[k].num, sizeof(int));
					}
				}while((sdrap != 1) && (semGet(sem_DispSrv, SRV_WRITE) != 1) && (sswitch != semGet(sem_DispSrv, SRV_SWITCH)));
				if(DISPMODE == 0){
					qsort(localStock.tabResult, 22, sizeof(TResults), (int (*)(const void*, const void*))cmpfunct);
					int i;
					system("clear");
					for(i = 0; i < 22; i++){
						if((i+1) < 10) printf("[0%d] | ", i+1);
						else printf("[%d] | ", i+1);
						if(localStock.tabResult[i].num < 10) printf("[Driver 0%d] ", localStock.tabResult[i].num); 
						else printf("[Driver %d] ", localStock.tabResult[i].num);
						printf("lap: %2d | time: %10.2lf sec | ",
								localStock.tabResult[i].lnum, localStock.tabResult[i].timeGlobal);
						printf("Retired : %s", localStock.tabResult[i].retired ? "yes" : "no");
						printf(" | Pitstop : %s\n", localStock.tabResult[i].pitstop ? "yes" : "no");
					}					
				}
			}
		} 
	}while(!finished);
	shmdt(&shm_DispSrv);
	semReset(sem_type, 0);
	fflush(stdin);
	printf("Press 0 to return to main menu...\n");
	while(getchar() != '0') fflush(stdin);
	showMainMenu();
}

void showTRMenu(int sem_control, int sem_type)
{
	fflush(stdin);
	system ( "clear" );
	printf("\033[36m");
	printf ("Trial Runs!\n");
	printf ("-------------------------------------\n\n");
	printf ("1 : Begin Trial Run 1\n");
	printf ("2 : Begin Trial Run 2\n");
	printf ("3 : Begin Trial Run 3\n");
	printf ("4 : Show Results of Trial Runs\n");
	printf ("0 : Back\n");
	printf ("-------------------------------------\n\n");
	printf("\033[0m");
	switch(getchar())
	{
		case '1' : show_notice("Monitor", "Trial 1 is going to begin");
				   sendSig(SIGTR1, sem_type, 0);
				   scoreMonitor(sem_control, SIGTR1);
				   break;
		case '2' : show_notice("Monitor", "Trial 2 is going to begin");
				   sendSig(SIGTR2, sem_type, 0);
				   scoreMonitor(sem_control, SIGTR2);
				   break;
		case '3' : show_notice("Monitor", "Trial 3 is going to begin");
				   sendSig(SIGTR3, sem_type, 0);
				   scoreMonitor(sem_control, SIGTR3);
				   break;
		case '4' : printf("Show Results\n"); break;
		case '0' : showMainMenu(); break;
		default  : showTRMenu(sem_control, sem_type); break;
	}
	fflush(stdin);
}

void showQualifMenu(int sem_control,  int sem_type)
{
	fflush(stdin);
	system ( "clear" );
	printf("\033[36m");
	printf ("Welcome to the worldest famous GPNKM!\n");
	printf ("-------------------------------------\n\n");
	printf ("1 : Begin Qualifier 1\n");
	printf ("2 : Begin Qualifier 2\n");
	printf ("3 : Begin Qualifier 3\n");
	printf ("4 : Show Results of Qualification Runs\n");
	printf ("0 : Back\n");
	printf ("-------------------------------------\n\n");
	printf("\033[0m");
	switch(getchar())
	{
		case '1' : show_notice("Monitor", "Qualification 1 is going to begin");
				   sendSig(SIGQU1, sem_type, 0);
				   scoreMonitor(sem_control, SIGQU1);
				   break;
		case '2' : show_notice("Monitor", "Qualification 1 is going to begin");
				   sendSig(SIGQU2, sem_type, 0);
				   scoreMonitor(sem_control, SIGQU2);
				   break;;
		case '3' : show_notice("Monitor", "Qualification 1 is going to begin");
				   sendSig(SIGQU3, sem_type, 0);
				   scoreMonitor(sem_control, SIGQU3);
				   break;
		case '4' : printf("Show Results of Qualifiers\n"); break;
		case '0' : showMainMenu(); break;
		default  : showQualifMenu(sem_control, sem_type); break;
	}   
	fflush(stdin);
}

void endOfProgram(int sem_control, int sem_type)
{
	key_t sem_DispSrv_key = ftok(PATH, STOCK);
	int sem_DispSrv = semget(sem_DispSrv_key, 2, IPC_CREAT | PERMS);
	sendSig(SIGEXIT, sem_control, 0);

	semctl(sem_type, 0, IPC_RMID, NULL);
	semctl(sem_DispSrv, SRV_SWITCH, IPC_RMID, NULL);
	semctl(sem_DispSrv, SRV_WRITE, IPC_RMID, NULL);
	key_t shm_DispSrv_key = ftok(PATH, STOCKSHM);
	int shm_DispSrv = shmget(shm_DispSrv_key, sizeof(TSharedStock), S_IRUSR | S_IWUSR);
	shmctl(shm_DispSrv, IPC_RMID, NULL);
	show_success("Monitor", "All processes closed successfully\nProgram will now exit");
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

void showMainMenu()
{
	key_t sem_type_key = ftok(PATH, TYPE);
	int sem_type = semget(sem_type_key, 1, IPC_CREAT | PERMS);
	
	key_t sem_control_key = ftok(PATH, CONTROL);
	int sem_control = semget(sem_control_key, 2, IPC_CREAT | PERMS);
	int weather = 1;
	
	while(!((weather >= SIGDRY) && (weather <= SIGRAIN))) weather = getSig(sem_control, 1);	
	
	fflush(stdin);
	system ( "clear" );
	printf("\033[36m");
	printf ("Welcome to the worldest famous GPNKM!\n");
	weatherMsg(weather);
	printf ("-------------------------------------\n\n");
	printf ("1 : Begin Test Runs\n");
	printf ("2 : Begin Qualifiers\n");
	printf ("3 : Begin Grand Prix\n");
	printf ("4 : Show Results of Grand Prix\n");
	printf ("5 : Restart Grand Prix\n");
	printf ("0 : Exit\n");
	printf ("-------------------------------------\n\n");
 	printf("\033[0m");
	switch(getchar())
	{
		case '1' : showTRMenu(sem_control, sem_type); break;
		case '2' : showQualifMenu(sem_control, sem_type); break;
		case '3' : show_notice("Monitor", "Grand Prix is going to begin");
				   sendSig(SIGGP, sem_type, 0);
				   scoreMonitor(sem_control, SIGGP);
				   break;
		case '4' : printf("Show Results\n"); break;
		case '5' : printf("Restart Grand Prix\n"); break;
		case '0' : endOfProgram(sem_control, sem_type); break;
		default  : showMainMenu(); break;
	}
    fflush(stdin);
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