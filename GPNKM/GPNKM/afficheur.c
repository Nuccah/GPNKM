#include "afficheur.h"

void showMainMenu()
{
	system ( "clear" );
	printf ("Welcome to the worldest famous GPNKM!\n");
	printf ("-------------------------------------\n\n");
	printf ("1 : Begin Test Runs\n");
	printf ("2 : Begin Qualifiers\n");
	printf ("3 : Begin Grand Prix\n");
	printf ("4 : Show Results of Grand Prix\n");
	printf ("5 : Restart Grand Prix\n");
	printf ("0 : Exit\n");
	printf ("-------------------------------------\n\n");
	char a = getchar();
	switch(a)
	{
		case '1' : showTRMenu(); break;
		case '2' : showQualifMenu(); break;
		case '3' : printf("Grand Prix Runs Begin\n"); break;
		case '4' : printf("Show Results\n"); break;
		case '5' : printf("Restart Grand Prix\n"); break;
		case '0' : printf("Boop\n"); break;
		default  : showMainMenu(); break;
	}
}

void showTRMenu()
{
	system ( "clear" );
	printf ("Test Runs!\n");
	printf ("-------------------------------------\n\n");
	printf ("1 : Begin Test Run 1\n");
	printf ("2 : Begin Test Run 2\n");
	printf ("3 : Begin Test Run 3\n");
	printf ("4 : Show Results of Trial Runs\n");
	printf ("0 : Back\n");
	printf ("-------------------------------------\n\n");
	char c = getchar();
	switch(c)
	{
		case '1' : printf("Test Run 1 Begin\n"); break;
		case '2' : printf("Test Run 2 Begin\n"); break;
		case '3' : printf("Test Run 3 Begin\n"); break;
		case '4' : printf("Show Results\n"); break;
		case '0' : showMainMenu(); break;
		default  : showTRMenu(); break;
	}
}

void showQualifMenu()
{
	system ( "clear" );
	printf ("Welcome to the worldest famous GPNKM!\n");
	printf ("-------------------------------------\n\n");
	printf ("1 : Begin Qualifier 1\n");
	printf ("2 : Begin Qualifier 2\n");
	printf ("3 : Begin Qualifier 3\n");
	printf ("4 : Show Results of Qualification Runs\n");
	printf ("0 : Exit\n");
	printf ("-------------------------------------\n\n");
	char b = getchar();
	switch(b)
	{
		case '1' : printf("Qualify Run 1 Begin\n"); break;
		case '2' : printf("Qualify Run 2 Begin\n"); break;
		case '3' : printf("Qualify Run 3 Begin\n"); break;
		case '4' : printf("Show Results of Qualifiers\n"); break;
		case '0' : showMainMenu(); break;
		default  : showQualifMenu(); break;
	}
}
