#include "gt.h"

int main(int argc, char** argv){
    int i, msqid, msqDissuasionid, shmid;
    srand(time(NULL));

    if((msqid = connectMsq(0)) != -1 && (msqDissuasionid = connectMsq(1)) != -1 && (shmid = connectShm()) != -1){

        config = shmat(shmid, config, 0);

        if(config != NULL){

            if(parseConfig(argc, argv, msqid, msqDissuasionid) != -1){

                /* Créátion du GT de dissuasion */
                pthread_create(&config->gt[65].thread, NULL, threadGtDissuasion, &config->gt[65]);

                /* Création des autres GT */
                for(i=0; i<config->nbGt; i++){
                    pthread_create(&config->gt[i].thread, NULL, threadGt, &config->gt[i]);
                }

                for(i=0; i<config->nbGt; i++){
                    pthread_join(config->gt[i].thread, NULL);
                }
            }
        }else{
            perror("Error: connexion au shm");
        }
    }   

    return 0;
}

int parseConfig(int argc, char** argv, int msqid, int msqDissuasionid){
    int i, j;
    FILE* configFile = NULL;

    if(argc >= 3){
        config->nbGt = atoi(argv[1]);

        if(config->nbGt > 0 && config->nbGt <= NB_GT-1){

            configFile = fopen(argv[2], "r");
            if(configFile != NULL){

                printf("PARSING GT_CONFIG FILE . . .\n");

                for(i=0; i<config->nbGt; i++){
                    config->gt[i].msqid = msqid;
                    config->gt[i].nbFiles = 0;
                    config->gt[i].idProd = 0;
                    config->gt[i].idCons = 0;
                    config->gt[i].nbMsg = 0;
                    config->gt[i].sizeTampon = SIZE_TAMPON;

                    fscanf(configFile, "%d,%d,%d,%d,%d,%d,%d", 
                        &config->gt[i].nbAgents, 
                        &config->gt[i].noFiles[0], 
                        &config->gt[i].noFiles[1], 
                        &config->gt[i].noFiles[2], 
                        &config->gt[i].noFiles[3], 
                        &config->gt[i].noFiles[4],
                        &config->gt[i].noFiles[5]);

                    for(j=0; j<6; j++){
                        if(config->gt[i].noFiles[j] > 0)          
                            config->gt[i].nbFiles++;
                    }

                    printf("GT%d # nbAgents=%d, nbFiles=%d, noFiles[0]=%d, noFiles[1]=%d, noFiles[2]=%d, noFiles[3]=%d, noFiles[4]=%d, noFiles[5]=%d\n",
                        config->gt[i].noFiles[0],
                        config->gt[i].nbAgents,
                        config->gt[i].nbFiles,
                        config->gt[i].noFiles[0],
                        config->gt[i].noFiles[1],
                        config->gt[i].noFiles[2],
                        config->gt[i].noFiles[3],
                        config->gt[i].noFiles[4],
                        config->gt[i].noFiles[5]);
                }

                fclose(configFile);
                printf("PARSING DONE\n");

                printf("CREATION DU GT DE DISSUASION\n");
                config->gt[65].msqid = msqDissuasionid;
                config->gt[65].noFiles[0] = 0;

                return 1;

            }else{
                perror("Error: open gt_config file");
            }
        }else{
            perror("Error: arg1 (Nombre de GT) > 0 et <= 64");
        }
    }else{
        perror("Error: arguments manquants, arg1 (Nombre GT), arg2 (Fichier de config)");  
    }        
    return -1;
}

void* threadGt(void* configGt){
    gt_t *gt = (gt_t*)configGt;
    msq_t msq;  
    int i;

    /* Création des agents */
    printf("GT%d # Création des agents . . .\n", gt->noFiles[0]);

    for(i=0; i<gt->nbAgents; i++){
        gt->agent[i].id = i+1;
        gt->agent[i].gt = gt;
        pthread_create(&gt->agent[i].thread, NULL, threadAgent, (void*)&gt->agent[i]);
    }

    /* Lancement du scheduler */
    printf("GT%d # Lancement du scheduler . . .\n", gt->noFiles[0]);
    
    while(1){
        /* On essaye de récupérer un message, récupération bloquante */
        if((rcvMsq(gt->msqid, gt->noFiles[0], &msq)) == 1){
            printf("GT%d # SCHEDULER : Appel mis en tampon\n", gt->noFiles[0]);
            pthread_mutex_lock(&gt->mutex);
            while(gt->nbMsg == gt->sizeTampon)
                pthread_cond_wait(&gt->nonPlein, &gt->mutex);
            gt->msqTampon[gt->idProd] = msq;
            gt->idProd = (gt->idProd+1)%gt->sizeTampon;
            (gt->nbMsg)++;
            pthread_mutex_unlock(&gt->mutex);
            pthread_cond_signal(&gt->nonVide);
        }
    }
}

void* threadGtDissuasion(void* configGt){
    gt_t *gt = (gt_t*)configGt;
    msq_t msq;

    while(1){
        if((rcvMsq(gt->msqid, gt->noFiles[0], &msq)) == 1){
            printf("GT Dissuasion # Dissuasion de l'appel.\n");
        }
    }

}

void* threadAgent(void* configAgent){
    agent_t *agent = (agent_t*)configAgent;
    gt_t *gt = agent->gt;
    msq_t msq;
    
    while(1){  
        /* Attente d'assignation d'un message */
        pthread_mutex_lock(&gt->mutex);
        while(gt->nbMsg == 0)
            pthread_cond_wait(&gt->nonVide, &gt->mutex);
        msq = gt->msqTampon[gt->idCons];
        gt->idCons = (gt->idCons+1)%gt->sizeTampon;
        (gt->nbMsg)--;
        pthread_mutex_unlock(&gt->mutex);
        pthread_cond_signal(&gt->nonPlein);

        /* Traitement appel */
        traitementAppel(gt, &msq, agent->id);
    }
}

void traitementAppel(gt_t *gt, msq_t *msq, int noAgent){
    printf("GT%d # AGENT %d : Traitement d'un appel -> OCCUPE\n", gt->noFiles[0], noAgent);
    printf("GT%d # AGENT %d : Type d'appel = %ld\n", gt->noFiles[0], noAgent, msq->mtype);
    printf("GT%d # AGENT %d : Nom appelant = %s\n", gt->noFiles[0], noAgent, msq->info.nom);
    printf("GT%d # AGENT %d : Numero d'appel = %lu\n", gt->noFiles[0], noAgent, msq->info.numero);
    printf("GT%d # AGENT %d : Durée d'appel = %d\n", gt->noFiles[0], noAgent, msq->info.dureeAppel);
    sleep(msq->info.dureeAppel);
    printf("GT%d # AGENT %d : Fin de l'appel -> PRET\n", gt->noFiles[0], noAgent);
}
