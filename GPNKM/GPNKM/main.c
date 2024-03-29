#include "main.h"

int main (int argc, char *argv[])
{
	// Sema for tansmit the type
	key_t sem_type_key = ftok(PATH, TYPE);
	int sem_type = semget(sem_type_key, 1, IPC_CREAT | PERMS);
	semReset(sem_type, 0);

	key_t sem_control_key = ftok(PATH, CONTROL);
	int sem_control = semget(sem_control_key, 2, IPC_CREAT | PERMS);
	semReset(sem_control, 0);
	semReset(sem_control, 1);	
	
	// Sema for control Shared Mem Stock
	key_t sem_DispSrv_key = ftok(PATH, STOCK);
	int sem_DispSrv = semget(sem_DispSrv_key, 1, IPC_CREAT | PERMS);
	semReset(sem_DispSrv, 0);

	// Shared mem between monitor and server
	key_t shm_DispSrv_key = ftok(PATH, STOCKSHM);
	int shm_DispSrv = shmget(shm_DispSrv_key, sizeof(TSharedStock), IPC_CREAT | PERMS); // Creation com display server shm

	// File Parameters
	/* Time Function for filename definition */
	time_t now = time(NULL);
	struct tm *time = localtime(&now);
	char date_time[30];
	strftime(date_time,sizeof(date_time),"%d%m%y_%H%M%S", time);

	pid_t process_id = fork(); // Premier Fork (Server, Afficheur)
	if (process_id < 0) {
		perror("Error while attempting Fork (Server/Afficheur de Resultat)");
		exit(19);
	}

	//Afficheur (Parent)//
	else if (process_id > 0){
		int level = 0;
		showMainMenu(level, date_time);
	} 
	/*Tampon Serveur (Child)*/
	else{
		// DAEMON CODE START //
		//daemonize();
		// PROCESS NOW A DAEMON //
		//***********//
		//*SEMA INIT*//
		//***********//
		// Sema for control shared mem race
		key_t sem_mutex_key = ftok(PATH, MUTEX);
		int sem_mutex = semget(sem_mutex_key, 1, IPC_CREAT | PERMS);
		semReset(sem_mutex, 0);

		key_t sem_race_key = ftok(PATH, RACE);
	    int sem_race = semget(sem_race_key, 22, IPC_CREAT | PERMS);
	    int r;
	    for(r = 0; r < 22; r++) semReset(sem_race, r);				
		//*****************//
		//*SHARED MEM INIT*//
		//*****************//
		key_t shm_race_key = ftok(PATH, RACESHM);
		int shm_race = shmget(shm_race_key, 22*sizeof(TTabCar), IPC_CREAT | PERMS);

		//***********//
		//*FORK INIT*//
		//***********//
		process_id = fork(); // Deuxieme Fork (Server, Pilot)
		if (process_id < 0) {
			perror("Error while attempting Fork (Server/Pilot)");
			exit(19);
		}
		//Pilots (Child)//
		else if (process_id == 0) forkPilots(); // Pilot forking function
		//Server (Parent)
		else server(date_time); // Main server function

		for(r = 0; r < 22; r++){
			if(semGet(sem_race, r) != 1) semReset(sem_race, r);
		 	semctl(sem_race, r, IPC_RMID, NULL);

		 }
		if(semGet(sem_mutex, 0) != 1) semReset(sem_mutex, 0);
		semctl(sem_mutex, 0, IPC_RMID, NULL);
		if(semGet(sem_type, 0) != 1) semReset(sem_type, 0);
		semctl(sem_type, 0, IPC_RMID, NULL);
		if(semGet(sem_control, 0) != 1) semReset(sem_control, 0);
		if(semGet(sem_control, 1) != 1) semReset(sem_control, 1);
		semctl(sem_control, 0, IPC_RMID, NULL);
		semctl(sem_control, 1, IPC_RMID, NULL);
		shmctl(shm_race, IPC_RMID, NULL);
		shmctl(shm_DispSrv, IPC_RMID, NULL);
		if(semGet(sem_DispSrv, 0) != 1) semReset(sem_DispSrv, 0);
		semctl(sem_DispSrv, 0, IPC_RMID, NULL);
	}
	return EXIT_SUCCESS;
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




