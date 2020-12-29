#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include "mtx_header.h"
#include "mtx.c"
#define MAX_RESOURCES 5
#ifndef SYS_gettid
#error "SYS_gettid unavailable on this system"
#endif
#define gettid() ((pid_t)syscall(SYS_gettid))

int available_resources = MAX_RESOURCES;
pthread_mutex_t mtx;

int decrease_count (int count)
{
    int ok = 0;
    mtxlock(0, gettid());
    if (available_resources < count)
    {
        printf("Couldn't get enough resources(%d).Retrying...\n",count);
        ok = -1;
    }
    else 
    {
        available_resources -= count;
        printf("Got %d resources %d remaining\n",count,available_resources);
    }
    mtxunlock(0);
    return ok;
}

int increase_count (int count)
{
    mtxlock(0, gettid());
    available_resources += count;
    printf("Released %d resources %d remaining\n",count,available_resources);
    mtxunlock(0);
}

void *zebra (void *arg) 
{
    int *nr = (int *) arg;
    while(decrease_count(*nr) == -1);
    increase_count(*nr);
    return NULL;
}

int main()
{
    if(mtxopen(getpid())) 
    {
        perror(NULL);
        return errno;
    }
    pthread_t *threads = malloc(sizeof(pthread_t) * 5); 
    int val[5] = {4,1,4,4,2};
    for (int i=0;i<5;i++)
        if (pthread_create(&threads[i],NULL,zebra,&val[i]))
        {
            perror(NULL);
            return errno;
        }
    
    for (int i=0;i<5;i++)
        if (pthread_join(threads[i],NULL))
        {
            perror(NULL);
            return errno;
        }
    //pthread_mutex_destroy(&mtx);
    mtxclose(0);
    return 0;
}
