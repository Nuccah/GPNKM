#include "structures.h"

void show_notice(const char *env, const char *msg){
	printf("\033[34m\n[%s] - N - %s\n", env, msg);
	printf("\033[0m");
}

void show_error(const char *env, const char *msg){
	printf("\033[31m\n[%s] - E - %s\n", env, msg);
	printf("\033[0m");
}

void show_debug(const char *env, const char *msg){
	printf("\033[33m\n[%s] - D - %s\n", env, msg);
	printf("\033[0m");
}

void show_success(const char *env, const char *msg){
	printf("\033[32m\n[%s] - S - %s\n", env, msg);
	printf("\033[0m");
}