#include "pilote.h"

int main(int argc, char** argv){
    int i;
    pthread_t pilote[NB_PILOTES];
    srand(time(NULL));

    if(argc >= 6){
    	int nbPilote = atoi(argv[1]);
        sleepSendBInfTime = atoi(argv[2]);
        sleepSendBSupTime = atoi(argv[3]);
        sleepMsgBInfTime = atoi(argv[4]);
        sleepMsgBSupTime = atoi(argv[5]);
        sleepSendBSupTime -= sleepSendBInfTime;
        sleepMsgBSupTime -= sleepMsgBInfTime;

    	if(nbPilote > 0 && nbPilote <= NB_PILOTES && sleepSendBInfTime > 0 && sleepMsgBInfTime > 0 && sleepSendBSupTime > 0 && sleepMsgBSupTime > 0){
            
            if((msqAttenteID = connectMsq(0)) != -1 && (msqDissuasionID = connectMsq(1)) != -1 && (shmID = connectShm()) != -1){

                config = shmat(shmID, config, 0);

                if(config != NULL){
                    for(i=0; i<nbPilote; i++){
                        pthread_create(&pilote[i], NULL, threadPilote, NULL);
                    }
                    
                    for(i=0; i<nbPilote; i++){
                        pthread_join(pilote[i], NULL);
                    }
                }else{
                    perror("Error: connexion au shm");
                }
            }
        }else{
            perror("Error: mauvais arguments, arg1 (pilote) entre 1 et 64, arg2 sleepSendBInfTime > 0, arg3 sleepSendBSupTime > sleepSendBInfTime, arg4 sleepMsgBInfTime > 0, arg5 sleepMsgBSupTime > sleepMsgBInfTime");
        }
    }else{
        perror("Error: arguments manquants, arg1 (pilote) entre 1 et 64, arg2 sleepSendBInfTime > 0, arg3 sleepSendBSupTime > sleepSendBInfTime, arg4 sleepMsgBInfTime > 0, arg5 sleepMsgBSupTime > sleepMsgBInfTime");
    }
    return 0;
}

void* threadPilote(){
    int noPilote = pthread_self()-1;
    int i = 0, j, k, envoye;
    msq_t msq;

    while(1){
        /*  Appel générée tout les x temps */
        sleep((rand()%sleepSendBSupTime)+sleepSendBInfTime);

        /* Création de l'appel */
        printf("Pilote %d # Nouvel appel entrant.\n", noPilote);
        msq.info.dureeAppel = (rand()%sleepMsgBSupTime)+sleepMsgBInfTime;
        sprintf(msq.info.nom, "%d name %d", noPilote, i);
        msq.info.numero = 33450200+noPilote;

        /* Recherche d'un GT disponible */ 
        envoye = 0;
        pthread_mutex_lock(&config->gt[noPilote-1].mutex);
        if(config->gt[noPilote-1].nbMsg == 0){
            /* Si le tampon dans le GT par défaut est vide */
            printf("Pilote %d # GT par défaut disponible, transmission de l'appel.\n", noPilote);
            msq.mtype = noPilote;
            envoye = 1;
            pthread_mutex_unlock(&config->gt[noPilote-1].mutex);
        }else{
            pthread_mutex_unlock(&config->gt[noPilote-1].mutex);
            /* Si le tampon dans le GT par défaut est plein, on cherche un GT de débordement */
            for(k=0; k<config->nbGt; k++){
                /* On parcours toute les files de débordement de chaque GT à la recheche d'un GT disposant de notre type d'appel */
                for(j=1; j<NB_FILE; j++){
                    if(config->gt[k].noFiles[j] == noPilote){
                        /* On réussis a trouver un GT de debordement, on regarde si son tampon est vide */
                        pthread_mutex_lock(&config->gt[k].mutex);
                        if(config->gt[k].nbMsg == 0){
                            /* Si le tampon du GT de débordement est vide, transmission de l'appel dans le GT de débordement */
                            printf("Pilote %d # GT par défaut débordé, transmission dans le GT de débordement %d.\n", noPilote, config->gt[k].noFiles[0]);
                            msq.mtype = config->gt[k].noFiles[0];
                            envoye = 1;
                        }
                        pthread_mutex_unlock(&config->gt[k].mutex);
                        if(envoye == 1) break;
                    }
                }
                if(envoye == 1) break;
            }
        }
        /* Si le GT par défaut et les GT de débordement sont pleins, on place l'appel dans le GT par défaut */
        if(envoye == 0){
            printf("Pilote %d # GT par défaut débordé, mais aucun GT de debordement disponible, transmission dans le GT par défaut.\n", noPilote);
            msq.mtype = noPilote;
        }

        /* Transmission de l'appel à la file d'attente */
        if(sendMsq(msqAttenteID, msq) == 1){
            printf("Pilote %d # Appel entrant transmis à la file d'attente.\n", noPilote);
        }else if(sendMsq(msqDissuasionID, msq) == 1){
            printf("Pilote %d # Appel entrant transmis à la file de dissuasion.\n", noPilote);
        }else{
            printf("Pilote %d # La file d'attente et la file dissuasion sont pleine. L'appel est rejeté.\n", noPilote);
        }

        i++;
    }
}
