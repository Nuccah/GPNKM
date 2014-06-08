#include "afficheur.h"

void scoreMonitor(int sem_control, int type){
	// INIT SECTION
	key_t sem_type_key = ftok(PATH, TYPE);
	int sem_type = semget(sem_type_key, 1, IPC_CREAT | PERMS);
	key_t sem_DispSrv_key = ftok(PATH, STOCK);
	int sem_DispSrv = semget(sem_DispSrv_key, 2, IPC_CREAT | PERMS);
	key_t shm_DispSrv_key = ftok(PATH, STOCKSHM);
	int shm_DispSrv = shmget(shm_DispSrv_key, sizeof(TSharedStock), S_IRUSR);
	key_t sem_modifa_key = ftok(PATH, MODIFA); // Sema Key generated
	int sem_modifa = semget(sem_modifa_key, 1, IPC_CREAT | PERMS); // sema ID containing 22 physical sema!!
	TSharedStock *listStock = (TSharedStock *) shmat(shm_DispSrv, NULL, 0); 
	waitSig(SIGSTART, sem_control, 0);
	printf("Run begins!!!\n\n");
	bool finished = false;
	TSharedStock localStock;
	do{
		if(checkSig(SIGEND, sem_control, 0)) finished = true;
		else{
			if(isShMemReadable(sem_modifa, 0)){
				semDown(sem_modifa,0);			
				if(isShMemReadable(sem_DispSrv, 0)){
					semDown(sem_DispSrv, DISP_READ);
					memcpy(&localStock, listStock, sizeof(TSharedStock));
					semUp(sem_DispSrv, DISP_READ);
					int i;
					system("clear");
					for(i = 0; i < 22; i++){
						if((i+1) < 10) printf("[0%d] | ", i+1);
						else printf("[%d] | ", i+1);
						if(localStock.tabResult[i].num < 10) printf("[Driver 0%d] ", localStock.tabResult[i].num); 
						else printf("[Driver %d] ", localStock.tabResult[i].num);
						printf("lap: %d | time: %.2lf sec | ",
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
	semctl(sem_DispSrv, DISP_READ, IPC_RMID, NULL);
	semctl(sem_DispSrv, SRV_WRITE, IPC_RMID, NULL);
	key_t shm_DispSrv_key = ftok(PATH, STOCKSHM);
	int shm_DispSrv = shmget(shm_DispSrv_key, sizeof(TSharedStock), S_IRUSR | S_IWUSR);
	shmctl(shm_DispSrv, IPC_RMID, NULL);
	show_success("Monitor", "All processes closed successfully\nProgram will now exit");
	semctl(sem_control, 0, IPC_RMID, NULL);
	exit(EXIT_SUCCESS);
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
	int sem_control = semget(sem_control_key, 1, IPC_CREAT | PERMS);
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
		case '0' : endOfProgram(sem_control, sem_type);
		default  : showMainMenu(); break;
	}
    fflush(stdin);
}
