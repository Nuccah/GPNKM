#include "afficheur.h"

void scoreMonitor(int queue_id, TmsgbufAdr adr_msg, int type, int sem_control, int sem_DispSrv, int shm_DispSrv)
{
	// INIT SECTION
	TSharedStock *listStock = (void *) shmat(shm_DispSrv, NULL, 0); 
	waitSig(SIGSTART, sem_control, 0);
	printf("Run begins!!!\n\n");
	//TODO implement this huge function ^^
	bool finished = false;
	TSharedStock localStock;
	do{
		if(checkSig(SIGEND, sem_control, 0)) finished = true;
		else{			
			if(isShMemReadable(sem_DispSrv, 0)){
				localStock = *listStock;
				int i;
				system("clear");
				for(i = 0; i < 22; i++){
					printf("[Driver %d] last lap time: %.2lf sec | global time: %.2lf sec\n", localStock.tabResult[i].num, 
							localStock.tabResult[i].timeLastLap, localStock.tabResult[i].timeGlobal);
				}
			}
		} 
	}while(!finished);
	shmdt(&shm_DispSrv);
}

void showTRMenu(int queue_id, TmsgbufAdr adr_msg, int sem_type, int sem_control, int sem_DispSrv, int shm_DispSrv)
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
				   printf("Race type sent: %d\n", getSig(sem_type, 0));
				   scoreMonitor(queue_id, adr_msg, SIGTR1, sem_control, sem_DispSrv, shm_DispSrv);
				   break;
		case '2' : show_notice("Monitor", "Trial 2 is going to begin");
				   sendSig(SIGTR2, sem_type, 0);
				   scoreMonitor(queue_id, adr_msg, SIGTR2, sem_control, sem_DispSrv, shm_DispSrv);
				   break;
		case '3' : show_notice("Monitor", "Trial 3 is going to begin");
				   sendSig(SIGTR3, sem_type, 0);
				   scoreMonitor(queue_id, adr_msg, SIGTR3, sem_control, sem_DispSrv, shm_DispSrv);
				   break;
		case '4' : printf("Show Results\n"); break;
		case '0' : showMainMenu(queue_id, adr_msg, sem_type, sem_control, sem_DispSrv, shm_DispSrv); break;
		default  : showTRMenu(queue_id, adr_msg, sem_type, sem_control, sem_DispSrv, shm_DispSrv); break;
	}
	fflush(stdin);
}

void showQualifMenu(int queue_id, TmsgbufAdr adr_msg,  int sem_type, int sem_control, int sem_DispSrv, int shm_DispSrv)
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
				   scoreMonitor(queue_id, adr_msg, SIGQU1, sem_control, sem_DispSrv, shm_DispSrv);
				   break;
		case '2' : show_notice("Monitor", "Qualification 1 is going to begin");
				   sendSig(SIGQU2, sem_type, 0);
				   scoreMonitor(queue_id, adr_msg, SIGQU2, sem_control, sem_DispSrv, shm_DispSrv);
				   break;;
		case '3' : show_notice("Monitor", "Qualification 1 is going to begin");
				   sendSig(SIGQU3, sem_type, 0);
				   scoreMonitor(queue_id, adr_msg, SIGQU3, sem_control, sem_DispSrv, shm_DispSrv);
				   break;
		case '4' : printf("Show Results of Qualifiers\n"); break;
		case '0' : showMainMenu(queue_id, adr_msg, sem_type, sem_control, sem_DispSrv, shm_DispSrv); break;
		default  : showQualifMenu(queue_id, adr_msg, sem_type, sem_control, sem_DispSrv, shm_DispSrv); break;
	}   
	fflush(stdin);
}

void endOfProgram(int queue_id, TmsgbufAdr adr_msg,  int sem_type, int sem_control, int sem_DispSrv, int shm_DispSrv)
{
	sendSig(SIGEXIT, sem_control, 0);

	semctl(sem_type, 0, IPC_RMID, NULL);
	msgctl(queue_id, IPC_RMID, NULL);
	semctl(sem_DispSrv, 0, IPC_RMID, NULL);
	shmdt(&shm_DispSrv);
	shmctl(shm_DispSrv, IPC_RMID, NULL);
	show_success("Monitor", "All processes closed successfully\nProgram will now exit");
	int stat = SIGTERM;
	wait(&stat);
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

void showMainMenu(int queue_id, TmsgbufAdr adr_msg, int sem_type, int sem_control, int sem_DispSrv, int shm_DispSrv)
{

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
		case '1' : showTRMenu(queue_id, adr_msg, sem_type, sem_control, sem_DispSrv, shm_DispSrv); break;
		case '2' : showQualifMenu(queue_id, adr_msg, sem_type, sem_control, sem_DispSrv, shm_DispSrv); break;
		case '3' : show_notice("Monitor", "Grand Prix is going to begin");
				   sendSig(SIGGP, sem_type, 0);
				   scoreMonitor(queue_id, adr_msg, SIGGP, sem_control, sem_DispSrv, shm_DispSrv);
				   break;
		case '4' : printf("Show Results\n"); break;
		case '5' : printf("Restart Grand Prix\n"); break;
		case '0' : endOfProgram(queue_id, adr_msg, sem_type, sem_control, sem_DispSrv, shm_DispSrv);
		default  : showMainMenu(queue_id, adr_msg, sem_type, sem_control, sem_DispSrv, shm_DispSrv); break;
	}
    fflush(stdin);
}
