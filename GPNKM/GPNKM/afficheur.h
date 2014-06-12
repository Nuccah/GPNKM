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
void afficheTrials(TTabQT *ResultsT1, TTabQT *ResultsT2, TTabQT *ResultsT3);
void afficheQualif(TTabQT *ResultsQ1, TTabQT *ResultsQ2, TTabQT *ResultsQ3);
void afficheGrandPrix(TTabGP *ResultsGP);

#endif