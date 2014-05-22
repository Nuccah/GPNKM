#include "afficheur.h"

void scoreMonitor(int queue_id, TmsgbufAdr adr_msg, int type){
	// INIT SECTION
	key_t sem_type_key = ftok(PATH, TYPE);
	int sem_type = semget(sem_type_key, 1, IPC_CREAT | PERMS);
	key_t sem_control_key = ftok(PATH, CONTROL);
	int sem_control = semget(sem_control_key, 1, IPC_CREAT | PERMS);
	key_t sem_DispSrv_key = ftok(PATH, STOCK);
	int sem_DispSrv = semget(sem_DispSrv_key, 2, IPC_CREAT | PERMS);
	key_t shm_DispSrv_key = ftok(PATH, STOCKSHM);
	int shm_DispSrv = shmget(shm_DispSrv_key, sizeof(TSharedStock), S_IRUSR);
	TSharedStock *listStock = (TSharedStock *) shmat(shm_DispSrv, NULL, 0); 
	waitSig(SIGSTART, sem_control, 0);
	printf("Run begins!!!\n\n");
	sleep(2);
	bool finished = false;
	TSharedStock localStock;
	do{
		if(checkSig(SIGEND, sem_control, 0)) finished = true;
		else{			
			printf("CA va\n");
			if(isShMemReadable(sem_DispSrv, 0)){
				semDown(sem_DispSrv, DISP_READ);
				localStock = *listStock;
				semUp(sem_DispSrv, DISP_READ);
				int i;
				system("clear");
				for(i = 0; i < 22; i++){
					printf("[Driver %d] lap: %d | time: %.2lf sec | ", localStock.tabResult[i].num, 
							localStock.tabResult[i].lnum, localStock.tabResult[i].timeLastLap);
					printf("Retired : %s\n", localStock.tabResult[i].retired ? "yes" : "no");
				}
			    sleep(1);
			}
		} 
	}while(!finished);
	shmdt(&shm_DispSrv);
	semReset(sem_type, 0);
}

void showTRMenu(int queue_id, TmsgbufAdr adr_msg, int sem_type)
{
	fflush(stdout);
//	system ( "clear" );
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
				   scoreMonitor(queue_id, adr_msg, SIGTR1);
				   break;
		case '2' : show_notice("Monitor", "Trial 2 is going to begin");
				   sendSig(SIGTR2, sem_type, 0);
				   scoreMonitor(queue_id, adr_msg, SIGTR2);
				   break;
		case '3' : show_notice("Monitor", "Trial 3 is going to begin");
				   sendSig(SIGTR3, sem_type, 0);
				   scoreMonitor(queue_id, adr_msg, SIGTR3);
				   break;
		case '4' : printf("Show Results\n"); break;
		case '0' : showMainMenu(queue_id, adr_msg); break;
		default  : showTRMenu(queue_id, adr_msg, sem_type); break;
	}
	fflush(stdin);
}

void showQualifMenu(int queue_id, TmsgbufAdr adr_msg,  int sem_type)
{
	fflush(stdout);
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
				   scoreMonitor(queue_id, adr_msg, SIGQU1);
				   break;
		case '2' : show_notice("Monitor", "Qualification 1 is going to begin");
				   sendSig(SIGQU2, sem_type, 0);
				   scoreMonitor(queue_id, adr_msg, SIGQU2);
				   break;;
		case '3' : show_notice("Monitor", "Qualification 1 is going to begin");
				   sendSig(SIGQU3, sem_type, 0);
				   scoreMonitor(queue_id, adr_msg, SIGQU3);
				   break;
		case '4' : printf("Show Results of Qualifiers\n"); break;
		case '0' : showMainMenu(queue_id, adr_msg); break;
		default  : showQualifMenu(queue_id, adr_msg, sem_type); break;
	}   
	fflush(stdin);
}

void endOfProgram(int queue_id, TmsgbufAdr adr_msg,  int sem_type)
{
	key_t sem_control_key = ftok(PATH, CONTROL);
	int sem_control = semget(sem_control_key, 1, IPC_CREAT | PERMS);
	key_t sem_DispSrv_key = ftok(PATH, STOCK);
	int sem_DispSrv = semget(sem_DispSrv_key, 2, IPC_CREAT | PERMS);
	sendSig(SIGEXIT, sem_control, 0);

	semctl(sem_type, 0, IPC_RMID, NULL);
	msgctl(queue_id, IPC_RMID, NULL);
	semctl(sem_DispSrv, DISP_READ, IPC_RMID, NULL);
	semctl(sem_DispSrv, SRV_WRITE, IPC_RMID, NULL);
	key_t shm_DispSrv_key = ftok(PATH, STOCKSHM);
	int shm_DispSrv = shmget(shm_DispSrv_key, sizeof(TSharedStock), S_IRUSR | S_IWUSR);
	shmctl(shm_DispSrv, IPC_RMID, NULL);
	show_success("Monitor", "All processes closed successfully\nProgram will now exit");
	semctl(sem_control, 0, IPC_RMID, NULL);
	exit(EXIT_SUCCESS);
}

void weather(int number){
	switch( number ) {
    	case 1: printf("It's a rainy day at the normally dry GPNKM track, drivers should prepare for a tough weekend!\n"); break;
    	case 2: printf("The weather has been off and on, drivers need to be ready for a wet circuit!\n"); break;
    	case 3: printf("It's a beautiful day at the GPNKM circuit, it is time to DRIVE!\n"); break;
	}
	return;
}

void showMainMenu(int queue_id, TmsgbufAdr adr_msg)
{
	key_t sem_type_key = ftok(PATH, TYPE);
	int sem_type = semget(sem_type_key, 1, IPC_CREAT | PERMS);
	fflush(stdout);
	system ( "clear" );
	printf("\033[36m");
	printf ("Welcome to the worldest famous GPNKM!\n");
	weather(adr_msg.weather);
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
		case '1' : showTRMenu(queue_id, adr_msg, sem_type); break;
		case '2' : showQualifMenu(queue_id, adr_msg, sem_type); break;
		case '3' : show_notice("Monitor", "Grand Prix is going to begin");
				   sendSig(SIGGP, sem_type, 0);
				   scoreMonitor(queue_id, adr_msg, SIGGP);
				   break;
		case '4' : printf("Show Results\n"); break;
		case '5' : printf("Restart Grand Prix\n"); break;
		case '0' : endOfProgram(queue_id, adr_msg, sem_type);
		default  : showMainMenu(queue_id, adr_msg); break;
	}
    fflush(stdin);
}
