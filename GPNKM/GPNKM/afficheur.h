#ifndef H_AFFICHEUR
#define H_AFFICHEUR
#include "structures.h"

void weatherMsg(int number);
void endOfProgram(int sem_control, int sem_type);
void scoreMonitor(int sem_control, int type, int level, char *date_time);
void showTRMenu(int sem_control, int sem_type, int level, char *date_time);
void showQualifMenu(int sem_control,  int sem_type, int level, char *date_time);
void showMainMenu(int level, char *date_time);
void afficheResultats(char *date_time, int level);
void afficheQT(TTabQT Results1, TTabQT Results2, TTabQT Results3, char *date_time, int level);
void afficheGrandPrix(TTabGP ResultsGP, char *date_time, int level);

#endif