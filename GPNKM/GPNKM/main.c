#include "main.h"

int main (int argc, char *argv[])
{
	int forked;
	int queue_id = msgget(42, 0666 | IPC_CREAT); // Creation de msg queue
	TmsgbufServ srv_msg;					 // Creation de msg queue
	TmsgbufPilot pilot_msg;					 // Creation de msg queue
	TmsgbufAdr adr_msg;


	// Sema for tansmit the type
	key_t sem_type_key = ftok(PATH, TYPE);
	int sem_type = semget(sem_type_key, 1, IPC_CREAT | PERMS);
	semReset(sem_type, 0);

	key_t sem_control_key = ftok(PATH, CONTROL);
	int sem_control = semget(sem_control_key, 1, IPC_CREAT | PERMS);
	semReset(sem_control, 0);

	// Sema for control Shared Mem Stock
	key_t sem_DispSrv_key = ftok(PATH, STOCK);
	int sem_DispSrv = semget(sem_DispSrv_key, 2, IPC_CREAT | PERMS);
	semReset(sem_DispSrv, DISP_READ);
	semReset(sem_DispSrv, SRV_WRITE);

	// Shared mem between monitor and server
	key_t shm_DispSrv_key = ftok(PATH, STOCKSHM);
	int shm_DispSrv = shmget(shm_DispSrv_key, sizeof(TSharedStock), IPC_CREAT | PERMS); // Creation com display server shm

	pid_t process_id = fork(); // Premier Fork (Server, Afficheur)
	if (process_id < 0) {
		perror("Error while attempting Fork (Server/Afficheur de Resultat)");
		exit(19);
	}

	//Afficheur (Parent)//
	else if (process_id > 0) {
		fflush(stdout);
		msgrcv(queue_id, &adr_msg, sizeof(struct TmsgbufAdr), ADR, 0);
		show_success("Monitor", "Server connected");
		showMainMenu(queue_id, adr_msg);
	}
	/*Tampon Serveur (Child)*/
	else{
		// DAEMON CODE START //
	//	daemonize();
		// PROCESS NOW A DAEMON //
		//***********//
		//*SEMA INIT*//
		//***********//
		// Sema for control shared mem race
		key_t sem_race_key = ftok(PATH, RACE); // Sema Key generated
		key_t sem_modif_key = ftok(PATH, MODIF); // Sema Key generated
		int sem_race = semget(sem_race_key, 22, IPC_CREAT | PERMS); // sema ID containing 22 physical sema!!
		int sem_modif = semget(sem_modif_key, 22, IPC_CREAT | PERMS); // sema ID containing 22 physical sema!!
		int i;
		for(i = 0; i < 22; i++)	semReset(sem_race, i);
		for(i = 0; i < 22; i++)	semReset(sem_modif, i);				
		//*****************//
		//*SHARED MEM INIT*//
		//*****************//
		key_t shm_race_key = ftok(PATH, RACESHM);
		int shm_race = shmget(shm_race_key, 22*sizeof(TCar), IPC_CREAT | PERMS); // Creation Race Shared Memory

		//***********//
		//*PIPE INIT*//
		//***********//
		int pfdSrvDrv[2]; int pfdDrvSrv[2];	pipe(pfdSrvDrv); pipe(pfdDrvSrv);	// Creation des pipes entre Serveur et les Pilotes
		process_id = fork(); // Deuxieme Fork (Server, Pilot)
		if (process_id < 0) {
			perror("Error while attempting Fork (Server/Pilot)");
			exit(19);
		}
		//Pilots (Child)//
		else if (process_id == 0) {
			close(pfdSrvDrv[1]);close(pfdDrvSrv[0]); // Close unused write/read ends of respective pipes
			forkPilots(queue_id, pfdSrvDrv[0], pfdDrvSrv[1], pilot_msg); // Pilot forking function
		}
		//Server (Parent)
		else{
			close(pfdSrvDrv[0]);close(pfdDrvSrv[1]); // Close unused write/read ends of respective pipes
			server(queue_id, pfdSrvDrv[1], pfdDrvSrv[0], adr_msg); // Main server function
			int stat = SIGTERM;
			wait(&stat); // Wait for any process returning SIGTERM
		}
		semctl(sem_race, 0, IPC_RMID, NULL);
		semctl(sem_type, 0, IPC_RMID, NULL);
		semctl(sem_control, 0, IPC_RMID, NULL);
		shmdt(&shm_race);
		shmctl(shm_race, IPC_RMID, NULL);
	}

//	tester();
	return 0;
}

void daemonize(){
	pid_t process_id = 0;
 	pid_t sid = 0;
 	process_id = fork();
 	if (process_id < 0)
 		exit(EXIT_FAILURE);
	if (process_id > 0)
	{
		int status = SIGTERM;
		wait(&status); // Wait for any process returning SIGTERM
		exit(EXIT_SUCCESS);
	}
	umask(0);
	sid = setsid(); // Set the session id (group process id)
	if(sid < 0)	exit(EXIT_FAILURE);
	chdir("/tmp"); // Better to run daemon processes in another dir than root
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}



/*
void tester(){
	int Drivers[] = {1,3,6,7,8,20,11,21,25,19,4,9,44,14,13,22,27,99,26,77,17,10};
	srand ( time(NULL) );
	int randDriver = rand() % 22;
	const char *weather = randomWeather();
	double speed = speedWeather(weather);
	const char *team = getTeamName(Drivers[randDriver]);
	double time1 = sectorTime(speed, S1);
	double time2 = sectorTime(speed, S2);
	double time3 = sectorTime(speed, S3);
	double lap1 = lapTime(time1, time2, time3);
	printf ("Welcome to the worldest famous GPNKM!\n");
	printf("Weather: %s \n", weather);
	printf("Weather Modified Speed: %.2lf \n", speed);
	printf("Name: %s \n", team);
	printf("Sector 1 Time: %.2lf \n", time1);
	printf("Sector 2 Time: %.2lf \n", time2);
	printf("Sector 3 Time: %.2lf \n", time3);
	printf("Laptime: %.2lf \n", lap1);
}*/





