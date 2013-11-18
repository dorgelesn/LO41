#ifndef LIBMSQ_H
#define LIBMSQ_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#define MSQ_KEY 1000

typedef struct {
    long mtype;
    struct info {
        char nom[20];
        unsigned long int numero;
        int dureeAppel;
    } info;
} msq_t;

int createMsq(int);
int deleteMsq(int);
int connectMsq(int);
int sendMsq(int, msq_t);
int rcvMsq(int, int, msq_t*);

#endif
