#include "structures.h"
#include "constants.h"
#include "serveur.h"
#include "afficheur.h"
#include "pilot.h"

void tester();
void daemonize();

int main (int argc, char *argv[])
{
	int forked;
	int queue_id = msgget(42, 0666 | IPC_CREAT); // Creation de msg queue
	TmsgbufServ srv_msg;					 // Creation de msg queue
	TmsgbufPilot pilot_msg;					 // Creation de msg queue
	TmsgbufAdr adr_msg;
	printf("111111\n");

	pid_t process_id = fork(); // Premier Fork (Server, Afficheur)
	if (process_id < 0) {
		perror("Error while attempting Fork (Server/Afficheur de Resultat)");
		exit(19);
	}

	//Afficheur (Parent)//
	else if (process_id > 0) {
		fflush(stdout);
		msgrcv(queue_id, &adr_msg, sizeof(struct TmsgbufAdr), ADR, 0);
//		int i;
//		for(i = 0; i < 23; i++) printf("Pid %d: %d\n", i, adr_msg.tabD[i]);
		showMainMenu(queue_id, adr_msg);
	}
	/*Tampon Serveur (Child)*/
	else{
		// DAEMON CODE START //
	//	daemonize();
		// PROCESS NOW A DAEMON //
		int gsmID = shmget(IPC_PRIVATE, 22*sizeof(TCar), IPC_CREAT | PERMS); // Creation Global Shared Memory
		TCar *tabCar = (void *) shmat(gsmID, NULL, 0); // Creation table shared by server and pilots
		int pfdSrvDrv[2]; int pfdDrvSrv[2];	pipe(pfdSrvDrv); pipe(pfdDrvSrv);	// Creation des pipes entre Serveur et les Pilotes
		process_id = fork(); // Deuxieme Fork (Server, Pilot)
		if (process_id < 0) {
			perror("Error while attempting Fork (Server/Pilot)");
			exit(19);
		}
		//Pilots (Child)//
		else if (process_id == 0) {
			close(pfdSrvDrv[1]);close(pfdDrvSrv[0]); // Close unused write/read ends of respective pipes
			forkPilots(queue_id, pfdSrvDrv[0], pfdDrvSrv[1], pilot_msg, tabCar); // Pilot forking function
			//pilot(number, queue_id, pfdSrvDrv[0], pfdDrvSrv[1], pilot_msg); // Fonction principale des pilotes
		}
		//Server (Parent)
		else{
			server(queue_id, pfdSrvDrv[1], pfdDrvSrv[0], adr_msg); // Fonction principale du serveur
			int stat = SIGTERM;
			wait(&stat); // Wait for any process returning SIGTERM
		}
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





