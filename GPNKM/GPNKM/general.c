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

// Reset the sema to 1
void semReset(int sem_id, int sem_channel){
	semctl(sem_id, sem_channel, SETVAL, 1);
}

// Check if shared mem is readable
bool isShMemReadable(int sem_id, int sem_channel){
	if(semctl(sem_id, sem_channel, GETVAL) == 1) return true;
	return false;
}


// sig = value to put in sema
// Put a sig in the corresponding channel of the corresponding sema
void sendSig(int sig, int sem_id, int sem_channel){
	semctl(sem_id, sem_channel, SETVAL, sig);
}

// force to wait the sig parameter in the corresponding sema
void waitSig(int sig, int sem_id, int sem_channel){
	while(semctl(sem_id, sem_channel, GETVAL) != sig); 
}

// check once the sig value in the sema and compare to the parameter 
// return true if they correspond else return false
bool checkSig(int sig, int sem_id, int sem_channel){
	if(semctl(sem_id, sem_channel, GETVAL) == sig) return true;
	return false;
}

// return the value of the signal in the sema
int getSig(int sem_id, int sem_channel){
	return semctl(sem_id, sem_channel, GETVAL);
}