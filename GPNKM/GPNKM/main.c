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
	int sem_DispSrv = semget(sem_DispSrv_key, 2, IPC_CREAT | PERMS);
	semReset(sem_DispSrv, SRV_SWITCH);
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
	else if (process_id > 0) showMainMenu();
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
		key_t sem_switch_key = ftok(PATH, SWITCH);
		int sem_switch = semget(sem_switch_key, 22, IPC_CREAT | PERMS);
		int sem_race = semget(sem_race_key, 22, IPC_CREAT | PERMS); // sema ID containing 22 physical sema!!

		key_t sem_ecr_key = ftok(PATH, SWITCH + 1);
		int sem_ecr = semget(sem_ecr_key, 22, IPC_CREAT | PERMS);

		key_t sem_lect_key = ftok(PATH, SWITCH + 2);
		int sem_lect = semget(sem_lect_key, 22, IPC_CREAT | PERMS);
		int i;
		for(i = 0; i < 22; i++){
			semReset(sem_race, i);
			semReset(sem_switch, i);
			semReset(sem_ecr, i);
			semReset(sem_lect, i);
		}				
		//*****************//
		//*SHARED MEM INIT*//
		//*****************//
		key_t shm_race1_key = ftok(PATH, RACESHM);
		int shm_race1 = shmget(shm_race1_key, 22*sizeof(TTabCar), IPC_CREAT | PERMS);

		key_t shm_race2_key = ftok(PATH, RACESHM + 1);
		int shm_race2 = shmget(shm_race2_key, 22*sizeof(TTabCar), IPC_CREAT | PERMS);

		key_t shm_race3_key = ftok(PATH, RACESHM + 2);
		int shm_race3 = shmget(shm_race3_key, 22*sizeof(TTabCar), IPC_CREAT | PERMS);

		//***********//
		//*PIPE INIT*//
		//***********//
		process_id = fork(); // Deuxieme Fork (Server, Pilot)
		if (process_id < 0) {
			perror("Error while attempting Fork (Server/Pilot)");
			exit(19);
		}
		//Pilots (Child)//
		else if (process_id == 0) forkPilots(); // Pilot forking function
		//Server (Parent)
		else server(); // Main server function

		for(i = 0; i < 22; i++){
			semctl(sem_race, i, IPC_RMID, NULL);
			semctl(sem_switch, i, IPC_RMID, NULL);
			semctl(sem_ecr, i, IPC_RMID, NULL);
			semctl(sem_lect, i, IPC_RMID, NULL);
		}
		semctl(sem_type, 0, IPC_RMID, NULL);
		semctl(sem_control, 0, IPC_RMID, NULL);
		semctl(sem_control, 1, IPC_RMID, NULL);
		shmctl(shm_race1, IPC_RMID, NULL);
		shmctl(shm_race2, IPC_RMID, NULL);
		shmctl(shm_race3, IPC_RMID, NULL);
		shmctl(shm_DispSrv, IPC_RMID, NULL);
		semctl(sem_DispSrv, SRV_WRITE, IPC_RMID, NULL);
		semctl(sem_DispSrv, SRV_SWITCH, IPC_RMID, NULL);
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




