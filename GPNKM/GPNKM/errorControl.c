#include "structures.h"
/*Personalisation of result in console mode to get a better vision to debug*/
void show_notice(const char *env, const char *msg){
	if(LOGLVL >= 1){
		printf("\033[34m\n[%s] - N - %s\033[0m \n", env, msg);
	}
}

void show_error(const char *env, const char *msg){
	if(LOGLVL >= 3){
		printf("\033[31m\n[%s] - E - %s\033[0m \n", env, msg);
	}
}

void show_debug(const char *env, const char *msg){
	if(LOGLVL >= 4){
		printf("\033[33m\n[%s] - D - %s\033[0m \n", env, msg);
	}
}

void show_success(const char *env, const char *msg){
	if(LOGLVL >= 2){
		printf("\033[32m\n[%s] - S - %s\033[0m \n", env, msg);
	}
}