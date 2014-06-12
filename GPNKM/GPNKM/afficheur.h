#ifndef H_AFFICHEUR
#define H_AFFICHEUR
#include "structures.h"

void weatherMsg(int number);
void endOfProgram(int sem_control, int sem_type);
void scoreMonitor(int sem_control, int type);
void showTRMenu(int sem_control, int sem_type);
void showQualifMenu(int sem_control,  int sem_type);
void showMainMenu();
void afficheResultats();
void afficheTrials();
void afficheQualif();
void afficheGrandPrix();

#endif