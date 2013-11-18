#include "libshm.h"

int createShm(){
	int shmid;
	if((shmid = shmget(SHM_KEY, sizeof(config_t), IPC_CREAT|0600)) == -1){
		perror("Error: création du shm");
		return -1;
	}
	return shmid;
}

int deleteShm(int shmid){
	if(shmctl(shmid, IPC_RMID, NULL) == -1){
     	perror("Error: suppression du shm");
     	return -1;
    }
    return 1;
}

int connectShm(){
	int shmid;
	if((shmid = shmget(SHM_KEY, sizeof(config_t), IPC_EXCL|0600)) == -1){
		perror("Error: connexion au shm");
		return -1;
	}
	return shmid;
}
