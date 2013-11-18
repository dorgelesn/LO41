#include "central.h"

int main(){

	int msqAttenteID, msqDissuasionID, shmID;

	if(((msqAttenteID = createMsq(0)) != -1) && ((msqDissuasionID = createMsq(1)) != -1) && ((shmID = createShm()) != -1)){
		manageMsq(msqAttenteID, msqDissuasionID, shmID);
	}

	return 0;
}

void manageMsq(int msqAttenteID, int msqDissuasionID, int shmID){
	int quit=0, choix;
	struct msqid_ds buf;

	while(quit != 1){
		printf("--------- MENU ---------\n");
		printf("1. État de la file d'attente\n");
		printf("2. État de la file de dissuasion\n");
		printf("3. Quitter sans supprimer les IPC\n");
		printf("4. Quitter en supprimant les IPC\n");
		printf("------------------------\n");
		printf("Entrez votre choix : ");
		scanf("%d",&choix);
		while(choix > 4 || choix < 1){
			printf("------------------------\n");
			printf("Le choix doit être entre 1 et 3.\n");
			printf("------------------------\n");
			printf("Entrez votre choix : ");
			scanf("%d",&choix);
		}
		printf("------------------------\n");
		switch(choix){
			case 1:
				if((msgctl(msqAttenteID, IPC_STAT, &buf)) == -1){
					perror("Error: accès à la file d'attente");
					break;
				}
				printf("File d'attente en ligne.\n");
				printf("Nombre de messages total : %d\n", (int)buf.msg_qnum);
				break;
			case 2:
				if((msgctl(msqDissuasionID, IPC_STAT, &buf)) == -1){
					perror("Error: accès à la file de dissuasion");
					break;
				}
				printf("File de dissuasion en ligne.\n");
				printf("Nombre de messages total : %d\n", (int)buf.msg_qnum);				
				break;
			case 3:
				quit = 1;
				break;
			case 4:
				deleteMsq(msqAttenteID);
				deleteMsq(msqDissuasionID);
				deleteShm(shmID);
				quit = 1;
				break;
		}
		printf("------------------------\n");
	}
}
