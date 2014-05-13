#include "structures.h"

// To use when you are going to work in shared memo
void semDown(int sem_id){
	struct sembuf op;
	op.sem_num = 0;
	op.sem_op = -1;
	op.sem_flg =  0;
	semop(sem_id, &op, 1);
} 

// To use when you finish your work in shared memo
void semUp(int sem_id){
	struct sembuf op;
	op.sem_num = 0;
	op.sem_op = 1;
	op.sem_flg = 0;
	semop(sem_id, &op, 1);
}
