#include "structures.h"

// To use when you are going to work in shared memo
void semDown(int sem_id, int sem_channel){
	struct sembuf op;
	op.sem_num = sem_channel;
	op.sem_op = -1;
	op.sem_flg =  0;
	semop(sem_id, &op, 1);
} 

// To use when you finish your work in shared memo
void semUp(int sem_id, int sem_channel){
	struct sembuf op;
	op.sem_num = sem_channel;
	op.sem_op = 1;
	op.sem_flg = 0;
	semop(sem_id, &op, 1);
}

// Check if shared mem is readable
bool isShMemReadable(int sem_id, int sem_channel){
	if(semctl(sem_id, sem_channel, GETVAL, 1) == 1) return true;
	return false;
}