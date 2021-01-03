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
char * name = "shm_daemon";
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
    //for (int i=1; i<30; i++)
    //int i=0;
    while(1)
    {
	int nr = mtxlist(mlist);
	for (int i=0; i< nr; i++)
	{
		if(mlist[i].wlist>0)
			mtxgrant(i);
	}
	syslog(LOG_NOTICE, "Daemon summoned.");
	sleep(1);
    }

    syslog(LOG_NOTICE, "Daemon finished.");
    closelog();
    return 0;
}
