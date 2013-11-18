#ifndef GT_H
#define GT_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include "libmsq.h"
#include "libshm.h"

#define NB_GT 65
#define NB_AGENT 64
#define NB_FILE 6
#define SIZE_TAMPON 1

struct Gt_t;

typedef struct {
	int id;							/* ID de l'agent */
	pthread_t thread;				/* Identificant du thread d'un agent */
	struct Gt_t* gt;				/* GT dans lequel se situe l'agent */
} agent_t;

typedef struct Gt_t {
	pthread_t thread; 				/* Identifiant du thread d'un GT */
	int msqid; 						/* File d'attente dans laquel il doit récuperer les appels */
	int nbFiles; 					/* Nombre de type de files que le GT peut traiter */
	int noFiles[NB_FILE]; 			/* Les types de files que le GT peut traiter */
	int nbAgents; 					/* Nombre d'agent appartenant au GT */
	agent_t agent[NB_AGENT]; 		/* Tableau des agents appartenant au GT */
	pthread_mutex_t mutex; 			/* Mutex pour l'algorithme du producteur-consomateur */
	pthread_cond_t nonPlein; 		/* Condition 1 pour l'algorithme du producteur-consomateur */
	pthread_cond_t nonVide; 		/* Condition 2 pour l'algorithme du producteur-consomateur */
	int idProd; 					/* Indice du producteur */
	int idCons; 					/* Indice du consommateur */
	int nbMsg; 						/* Nombre de message dans le tampon */
	int sizeTampon; 				/* Taille du tampon */
	msq_t msqTampon[SIZE_TAMPON]; 	/* Tampon */
} gt_t;

typedef struct {
	int nbGt;						/* Nombre de GT */
	gt_t gt[NB_GT];					/* Tableau des GTs */
} config_t;

config_t *config;

int parseConfig(int, char**, int, int);
void* threadGt(void*);
void* threadGtDissuasion(void*);
void* threadAgent(void*);
void traitementAppel(gt_t*, msq_t*, int);

#endif
