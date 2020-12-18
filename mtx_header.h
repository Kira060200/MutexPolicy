#ifndef MTX_FUNCTIONS
#define MTX_FUNCTIONS
#include<pthread.h>

int mtxopen(pid_t pid);
int mtxclose(int id);
int mtxlock(int id, pid_t pid);
int mtxunlock(int id);
int mtxlist(pid_t *list, int nlist);
int mtxgrant(int id);
#endif
