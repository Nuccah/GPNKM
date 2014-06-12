#ifndef H_AFFICHEUR
#define H_AFFICHEUR
#include "structures.h"

void weatherMsg(int number);
void endOfProgram(int sem_control, int sem_type);
void scoreMonitor(int sem_control, int type, int level);
void showTRMenu(int sem_control, int sem_type, int level);
void showQualifMenu(int sem_control,  int sem_type, int level);
void showMainMenu(int level);
void afficheResultats();
void afficheTrials();
void afficheQualif();
void afficheGrandPrix();

#endif