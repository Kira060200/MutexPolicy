#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include "mtx_header.h"
#include "mtx.c"
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#define MAX_RESOURCES 5
#ifndef SYS_gettid
#error "SYS_gettid unavailable on this system"
#endif
#define gettid() ((pid_t)syscall(SYS_gettid))
int available_resources = MAX_RESOURCES;
pthread_mutex_t mtx; 
pthread_t threads[5];
int decrease_count(int count)
{
	printf("PID: %d\n",gettid());
	mtxlock(0, gettid());
	if (available_resources < count)
	{
		mtxunlock(0);
		return -1;
	}
	else
		available_resources -= count;
	printf("Got %d resources %d remaining\n", count, available_resources);
	mtxunlock(0);
	return 0;

}
int increase_count(int count)
{
	printf("PID: %d\n",gettid());
	mtxlock(0, gettid());
	available_resources += count;
	printf("Released %d resources %d remaining\n", count, available_resources);
	mtxunlock(0);
	return 0;
}
void *calc(void *v)
{
	int count = (int)v;
	decrease_count(count);
	increase_count(count);
	return 0;
}
int main()
{
	printf("MAX_RESOURCES=%d\n", MAX_RESOURCES);
	if(mtxopen(getpid())) 
	{
		perror(NULL);
		return errno;
	}
	int i = 0;
	for (i = 0; i < 5; i++)
	{
		int val = rand() % (MAX_RESOURCES + 1);
		if(pthread_create(&threads[i], NULL, calc, val))
		{
			perror(NULL);
			return errno;
		}
	}
	for (i = 0; i < 5; i++)
	{
		if(pthread_join(threads[i], NULL))
		{
			perror(NULL);
			return errno;
		}
	}
	//pthread_mutex_destroy(&mtx);
	return 0;
}

