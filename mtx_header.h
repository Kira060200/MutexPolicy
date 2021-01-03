#ifndef MTX_FUNCTIONS
#define MTX_FUNCTIONS
#include<pthread.h>
#include<stdbool.h>
struct mutex
{
	int id;
	bool opened;
	bool locked;
	pid_t * wlist;
	int nr_wlist;
};
int mtxopen(pid_t pid, int id);
int mtxclose(int id);
int mtxlock(int id, pid_t pid);
int mtxunlock(int id);
int mtxlist(struct mutex *list);
int mtxgrant(int id);
#endif
