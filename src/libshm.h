#ifndef LIBSHM_H
#define LIBSHM_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include "gt.h"

#define SHM_KEY 2000

int createShm();
int deleteShm(int);
int connectShm();

#endif
