#ifndef MTX_FUNCTIONS
#define MTX_FUNCTIONS
#include<pthread.h>

int mtxopen(pid_t pid);
int mtxclose(int id, pid_t pid);
int mtxlock(int id, pid_t pid);
int mtxunlock(int id, pid_t pid);
int mtxlist(pid_t *list, int nlist);
int mtxgrant(int id, pid_t gr);
#endif
