#include "afficheur.h"

void showTRMenu(int queue_id, TmsgbufAdr adr_msg)
{
	fflush(stdout);
	system ( "clear" );
	printf ("Test Runs!\n");
	printf ("-------------------------------------\n\n");
	printf ("1 : Begin Test Run 1\n");
	printf ("2 : Begin Test Run 2\n");
	printf ("3 : Begin Test Run 3\n");
	printf ("4 : Show Results of Trial Runs\n");
	printf ("0 : Back\n");
	printf ("-------------------------------------\n\n");
	switch(getchar())
	{
		case '1' : printf("Test Run 1 Begin\n"); break;
		case '2' : printf("Test Run 2 Begin\n"); break;
		case '3' : printf("Test Run 3 Begin\n"); break;
		case '4' : printf("Show Results\n"); break;
		case '0' : showMainMenu(queue_id, adr_msg); break;
		default  : showTRMenu(queue_id, adr_msg); break;
	}
	fflush(stdin);
}

void showQualifMenu(int queue_id, TmsgbufAdr adr_msg)
{
	fflush(stdout);
	system ( "clear" );
	printf ("Welcome to the worldest famous GPNKM!\n");
	printf ("-------------------------------------\n\n");
	printf ("1 : Begin Qualifier 1\n");
	printf ("2 : Begin Qualifier 2\n");
	printf ("3 : Begin Qualifier 3\n");
	printf ("4 : Show Results of Qualification Runs\n");
	printf ("0 : Back\n");
	printf ("-------------------------------------\n\n");
	switch(getchar())
	{
		case '1' : printf("Qualify Run 1 Begin\n"); break;
		case '2' : printf("Qualify Run 2 Begin\n"); break;
		case '3' : printf("Qualify Run 3 Begin\n"); break;
		case '4' : printf("Show Results of Qualifiers\n"); break;
		case '0' : showMainMenu(queue_id, adr_msg); break;
		default  : showQualifMenu(queue_id, adr_msg); break;
	}
	fflush(stdin);
}

void endOfProgram(int queue_id, TmsgbufAdr adr_msg)
{
	int i;
	for(i = 0; i < 23; i++) kill(adr_msg.tabD[i], SIGTERM);
	printf("All processes closed successfully\n");
	msgctl(queue_id, IPC_RMID, NULL);
	exit(EXIT_SUCCESS);
}

void showMainMenu(int queue_id, TmsgbufAdr adr_msg)
{
	fflush(stdout);
	//system ( "clear" );
	printf ("Welcome to the worldest famous GPNKM!\n");
	printf ("-------------------------------------\n\n");
	printf ("1 : Begin Test Runs\n");
	printf ("2 : Begin Qualifiers\n");
	printf ("3 : Begin Grand Prix\n");
	printf ("4 : Show Results of Grand Prix\n");
	printf ("5 : Restart Grand Prix\n");
	printf ("0 : Exit\n");
	printf ("-------------------------------------\n\n");
	switch(getchar())
	{
		case '1' : showTRMenu(queue_id, adr_msg); break;
		case '2' : showQualifMenu(queue_id, adr_msg); break;
		case '3' : printf("Grand Prix Runs Begin\n"); break;
		case '4' : printf("Show Results\n"); break;
		case '5' : printf("Restart Grand Prix\n"); break;
		case '0' : endOfProgram(queue_id, adr_msg);
		default  : showMainMenu(queue_id, adr_msg); break;
	}
    fflush(stdin);
}
