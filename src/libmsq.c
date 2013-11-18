#include "libmsq.h"

int createMsq(int key){
	int msqid;
	if((msqid = msgget(MSQ_KEY+key, IPC_CREAT|0600)) == -1){
		perror("Error: création de la file de messages");
		return -1;
    }
    return msqid;
}

int deleteMsq(int msqid){
	if(msgctl(msqid, IPC_RMID, NULL) == -1){
     	perror("Error: suppression de la file de messages");
     	return -1;
    }
    return 1;
}

int connectMsq(int key){
    int msqid;
    if((msqid = msgget(MSQ_KEY+key, IPC_EXCL|0600)) == -1){
    	perror("Error: connexion a la file de messages");
        return -1;
    }
    return msqid;
}

int sendMsq(int msqid, msq_t msq){
    msq_t msqToSend = msq;
	if(msgsnd(msqid, &msqToSend, sizeof(msq_t), IPC_NOWAIT) == -1){
        if(errno == EAGAIN){
            return -1;
        }
    	perror("Error: ecriture dans la file de message");
    	exit(0);
    }
    return 1;
}

int rcvMsq(int msqid, int noGt, msq_t *msq){
    if((msgrcv(msqid, msq, sizeof(msq_t), noGt, 0)) == -1){
        if(errno == ENOMSG){
            return -1;
        }
        perror("Error: lecture dans la file de message");
        exit(0);
    }
    return 1;
}
