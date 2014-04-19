#include "structures.h"
#include "constants.h"
#include "serveur.h"
#include "afficheur.h"
#include "pilot.h"

void tester();

int main (int argc, char *argv[])
{
	setsid
	;
	int pfdSrvAdR[2]; // Creation du pipe entre Serveur et Afficheur du Resultat
	int pfdAdR[2]; // Creation du pipe entre Serveur et Afficheur du Resultat
	pipe(pfdSrvAdR);	 // Creation du pipe entre Serveur et Afficheur du Resultat
	pipe(pfdAdR);	 // Creation du pipe entre Serveur et Afficheur du Resultat
	int forked;
	forked = fork(); // Premier Fork (Server, Afficheur)
	if (forked < 0) {
		perror("Error while attempting Fork (Server/Afficheur de Resultat)");
		exit(19);
	}
	/*Server (Parent)*/
	else if (forked > 0) {    
		int pfdSrvDrv[2]; 	// Creation des pipes entre Serveur et les Pilotes
		int pfdDrv[2];	// Creation des pipes entre Serveur et les Pilotes
		pipe(pfdSrvDrv);		// Creation des pipes entre Serveur et les Pilotes
		pipe(pfdAdR);		// Creation des pipes entre Serveur et les Pilotes
		forked = fork(); // Deuxieme Fork (Server, Pilot)
		int drivers[] = {1,3,6,7,8,20,11,21,25,19,4,9,44,14,13,22,27,99,26,77,17,10}; // Tableau contenant les #'s des conducteurs
		if (forked < 0) {
			perror("Error while attempting Fork (Server/Pilot)");
			exit(19);
		}
		/*Server (Parent)*/
		else if (forked > 0) {
			int i;
			close(pfdSrvAdR[0]); // Close unused read end
			close(pfdAdR[1]); // Close unused write end  
			const char *weather = randomWeather(); // Weather Selection
			printf("Weather: %s \n", weather);
			for(i=1;i<(sizeof(drivers) / sizeof(int))+1;i++){ // Write in pipe all available numbers
				write(pfdSrvDrv[1], &drivers[i-1], sizeof(int));
			} 
		}
		/*Pilot (Child)*/
		else{ 
			int i;
			close(pfdSrvAdR[0]);close(pfdSrvAdR[1]); // Close pipe of Server->Afficheur for Pilots
			close(pfdAdR[0]);close(pfdAdR[1]); // Close pipe of Afficheur->Server for Pilots
			close(pfdSrvDrv[1]);close(pfdDrv[1]); // Close unused write/read ends of respective pipes
			int pid[(sizeof(drivers) / sizeof(int))]; // ????
			printf("%ld\n", sizeof(pid));
			for(i=1;i<(sizeof(pid)-1);i++){ // Multifork des 22 pilotes
      			if((pid[i]=fork()) == -1){
         			printf("Error while attempting Fork (Pilot/Pilot)");
          			exit(1);
        		}
      			else if(pid[i]>0){ // DRIVERS //
					int number;
          			printf(" \n pid: %3d\n",pid[i]);
          			read(pfdSrvDrv[0], &number, sizeof(int)); // First come first serve for driver numbers in pipe
          			const char *team = getTeamName(number); // Return team name according to driver number
          			printf("Number: %d - Team: %s \n",number, team);
        		}
    		} 
		}
	}
	/*Afficheur de Resultat (Child)*/
	else{ 
		close(pfdSrvAdR[1]); // Close unused write end
		close(pfdAdR[0]); // Close unused read end
		showMainMenu();
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





