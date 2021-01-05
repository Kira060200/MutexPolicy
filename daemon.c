#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include "mtx_header.h"
#include "mtx.c"

struct mutex * mlist;
int ret;
void handle(int sign)
{
	if (sign == SIGTERM)
	{
		syslog(LOG_NOTICE, "sigterm");
		exit(0);
	}
	else if (sign == SIGHUP)
	{
		syslog(LOG_NOTICE, "sighup");
	}
}
int main()
{
    pid_t sid, pid;
    openlog("my_daemon", LOG_PID, LOG_DAEMON);
    pid = fork();
    if (pid < 0)
    {
        syslog(LOG_NOTICE, "pid error");
        return errno;
    }
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }
    umask(0);
    sid = setsid();
    if (sid < 0)
    {
        syslog(LOG_NOTICE, "sid error");
        return errno;
    }
    signal(SIGTERM, handle);
    signal(SIGHUP, handle);

    if(chdir("/") < 0)
    {
        perror(NULL);
        return errno;
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    shm_fd = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(shm_fd < 0)
    {
	    syslog(LOG_NOTICE, "Failed to create shared memory");
	    perror(NULL);
	    return errno;
    }
    else
    {
	    size_t shm_size = getpagesize()*50;
	    ret = ftruncate(shm_fd, shm_size);
	    if(ret == -1)
	    {
		perror(NULL);
		shm_unlink(name);
		return errno;
	    }
    }
    //for (int i=1; i<30; i++)
    //int i=0;
    //char* shm_ptr = mmap(0, getpagesize(), PROT_READ, MAP_SHARED, shm_fd, getpagesize()*0);
    //printf("%s",shm_ptr);
    //syslog(LOG_NOTICE, "%s\n", shm_ptr);
    //munmap(shm_ptr, getpagesize());
    //while(1)
    int v[256];
    int nr_list=0;
    syslog(LOG_NOTICE, "Daemon summoned.");
    for (int i=1; i<30; i++)
    {
	nr_list = mtxlist(v);
	//nr_list = sizeof v / sizeof v[0];
	syslog(LOG_NOTICE, "%d\n", nr_list);
	for(int j=0; j<nr_list; j++)
	{
		mtxgrant(v[j]);
		syslog(LOG_NOTICE, "%d\n", v[j]);
	}
	//int nr = mtxlist(mlist);
	//for (int i=0; i< nr; i++)
	//{
	//	if(mlist[i].wlist>0)
	//		mtxgrant(i);
	//}
	syslog(LOG_NOTICE, "Daemon summoned.");
	sleep(1);
    }

    syslog(LOG_NOTICE, "Daemon finished.");
    closelog();
    shm_unlink(name);
    return 0;
}
