#ifndef PILOTE_H
#define PILOTE_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "libmsq.h"
#include "libshm.h"
#include "gt.h"

#define NB_PILOTES 64

int msqAttenteID, msqDissuasionID, shmID, sleepSendBInfTime, sleepSendBSupTime, sleepMsgBInfTime, sleepMsgBSupTime;
config_t *config;

void* threadPilote();

#endif
