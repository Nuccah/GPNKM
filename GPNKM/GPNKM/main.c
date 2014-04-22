#include "structures.h"
#include "constants.h"
#include "serveur.h"
#include "afficheur.h"
#include "pilot.h"

void tester();

int main (int argc, char *argv[])
{
	int forked;
	int queue_id = msgget(42, 0666 | IPC_CREAT); // Creation de msg queue
	struct msgbufSrv* srv_msg;					 // Creation de msg queue
	struct msgbufPilot pilot_msg;					 // Creation de msg queue
	struct msgbufPilot pilot_msg2;
	int rcRcv, rcSnd;							 // Creation de msg queue
	forked = fork(); // Premier Fork (Server, Afficheur)
	if (forked < 0) {
		perror("Error while attempting Fork (Server/Afficheur de Resultat)");
		exit(19);
	}

	//Afficheur (Parent)//
	else if (forked > 0) {
		do{
			fflush(stdout);

			printf("1\n");
			printf("2\n");
			rcRcv = msgrcv(queue_id, &pilot_msg2, sizeof(struct msgbufPilot), 0, 0);	
			printf("Number Received: %d", pilot_msg2.car.num);
			showMainMenu();
		}while(1);
	}
	/*Tampon Serveur (Child)*/
	else{
		// DAEMON CODE START //
		pid_t process_id = 0;
 		pid_t sid = 0;
 		process_id = fork();
 		if (process_id < 0)
 			exit(EXIT_FAILURE);
		if (process_id > 0)
		{
			int status = 0;
			waitpid(process_id, &status, 0);
			exit(EXIT_SUCCESS);
		}
		umask(0);
		sid = setsid();
		if(sid < 0)
			exit(EXIT_FAILURE);
		chdir("/tmp"); // Better to run daemon processes in another dir than root
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		// PROCESS NOW A DAEMON //
		int pfdSrvDrv[2]; 	// Creation des pipes entre Serveur et les Pilotes
		int pfdDrvSrv[2];	// Creation des pipes entre Serveur et les Pilotes
		pipe(pfdSrvDrv);		// Creation des pipes entre Serveur et les Pilotes// Creation des pipes entre Serveur et les Pilotes
		pipe(pfdDrvSrv);
		int drivers[] = {1,3,6,7,8,20,11,21,25,19,4,9,44,14,13,22,27,99,26,77,17,10}; // Tableau contenant les #'s des conducteurs
		int forked2 = fork(); // Deuxieme Fork (Server, Pilot)
		if (forked2 < 0) {
			perror("Error while attempting Fork (Server/Pilot)");
			exit(19);
		}
		//Pilots (Child)//
		else if (forked2 == 0) {
			close(pfdSrvDrv[1]);close(pfdDrvSrv[0]); // Close unused write/read ends of respective pipes
			int number = forkPilots(sizeof(drivers)/sizeof(int), pfdSrvDrv[0], pfdDrvSrv[1]);
			struct TCar pilot = {0};
			pilot.num = number; 
			pilot_msg.mtype = 1; 
			pilot_msg.car = pilot;
			rcSnd = msgsnd(queue_id, &pilot_msg, sizeof(struct msgbufPilot), 0);
			do{
				sleep(5);
			}while(1);
		}
		//Server (Parent) *DEFUNCT*//
		else{
			int i;
			int pidDrivers[22];
			const char *weather = randomWeather(); // Weather Selection
			//printf("Weather: %s \n", weather);
			for(i=1;i<(sizeof(drivers) / sizeof(int))+1;i++){ // Write in pipe all available numbers
				write(pfdSrvDrv[1], &drivers[i-1], sizeof(int));
			}
			for(i=1;i<(sizeof(drivers) / sizeof(int))+1;i++){ // Read in pipe all driver pids
				read(pfdDrvSrv[0], &pidDrivers[i-1], sizeof(int));
			}
			do{
				sleep(5);
			}while(1);
		}
		// Temporary Loop //
		do{
			sleep(5);
		}while(1);
	}
//	tester();
	return 0;
}

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
}





