#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "mtx_header.h"

bool created_list = false;
int nr_mlist = 0;
int nr_plist = 0;
pid_t wake_process;
struct process
{
	pid_t id;
	bool status; //1 for waiting, else 0
	int * h_mlist;
	int h_mlist_size;
};
struct mutex
{
	int id;
	bool opened;
	bool locked;
	pid_t * wlist;
	int nr_wlist;
};
struct process * plist;
struct mutex * mlist;

int mtxopen(pid_t pid, int id)
{
	if (!created_list)
	{
		created_list = true;
		mlist = malloc(sizeof(struct mutex));
		mlist[0].locked = false;
		mlist[0].opened = false;
		mlist[0].id = nr_mlist;
		nr_mlist = 1;
	}
	// else
	// {
	// 	mlist = realloc(mlist, sizeof(struct mutex)*(nlist+1));
	// 	nlist ++;
	// }
	struct process *p = malloc(sizeof(struct process));
	p->id = pid;
	p->status = false;
	bool available = false;
	for (int i = 0; i<nr_mlist; i++)
	{
		if(!mlist[i].opened && mlist[i].id == id)
		{
			// TO DO: daca trece de 2 ori prin acelasi mutex il trece de 2 ori :(
			p->h_mlist = realloc(p->h_mlist, sizeof(int*)*(p->h_mlist_size + 1));
			p->h_mlist[p->h_mlist_size] = mlist[i].id;
			p->h_mlist_size ++;
			mlist[i].locked = false;
			mlist[i].opened = true;
			available = true;
			break;
		}
	}
	if(!available)
	{
		mlist = realloc(mlist, sizeof(struct mutex)*(nr_mlist+1));
		mlist[nr_mlist].opened = true;
		mlist[nr_mlist].locked = false;
		mlist[nr_mlist].id = nr_mlist;
		p->h_mlist = realloc(p->h_mlist, sizeof(int*)*(p->h_mlist_size + 1));
		p->h_mlist[p->h_mlist_size] = mlist[nr_mlist].id;
		p->h_mlist_size ++;
		nr_mlist++;
	}
	return 0;
}

int mtxclose(int id)
{
	if (created_list)
	{
		for (int i = 0; i<nr_mlist; i++)
		{
			if(mlist[i].id == id)
			{
				mlist[i].opened = false;
				printf("nr_mlist %d\n", nr_mlist);
				printf("nr_wlist %d\n", mlist[id].nr_wlist);
				// for(int j=i; j<nlist-1; j++)
				// 	mlist[j]=mlist[j+1];
				// mlist = realloc(mlist, sizeof(struct mutex)*(--nlist));
				return 0;
			}
		}
	}
	return -1;
}

int mtxlock(int id, pid_t pid)
{
	int mutex_found = -1;
	int process_found = -1;
	if(created_list)
	{
		//plist = realloc(plist, sizeof(struct process)*(nr_plist+1));
		//plist[nr_plist].id = pid;
		//nr_plist++;
		//plist[nr_mlist].
		//printf("Nr plist %d with pid %d\n",nr_plist, pid);
		for(int i=0; i<nr_mlist; i++)
		{
			if(mlist[i].id == id && mlist[i].opened == true)
			{
				mutex_found = i;
				break;
			}
		}
		if(mutex_found == -1)
			return -1;
		// nr_plist = sizeof(&plist) / sizeof(struct process);
		//printf("Found mutex %d\n",mutex_found);
		/*for(int i=0; i<nr_plist; i++)
			if(plist[i].id == pid)
			{
				process_found = i;
				break;
			}
		if(process_found == -1)
			return -1;
		plist[process_found].status = true;*/
			//printf("Nr of pr waiting before me: %d\n", mlist[mutex_found].nr_wlist);
		if(mlist[mutex_found].locked == false)
		{
				//printf("lock PID: %d\n", pid);
				//printf("Mutex not locked %d\n", mlist[mutex_found].nr_wlist);
			//mlist[mutex_found].wlist = realloc(mlist[mutex_found].wlist, sizeof(pid_t)*(mlist[mutex_found].nr_wlist++));
			//mlist[mutex_found].wlist[mlist[mutex_found].nr_wlist-1] = pid;
			//printf("%d", mlist[mutex_found].nr_wlist);
			mlist[mutex_found].locked = true;
			//plist[process_found].status = false;
			return 0;
		}
			//printf("lock PID: %d with nr_wlist: %d\n", pid, mlist[mutex_found].nr_wlist);
		mlist[mutex_found].wlist = realloc(mlist[mutex_found].wlist, sizeof(pid_t)*(mlist[mutex_found].nr_wlist+1));
		mlist[mutex_found].wlist[mlist[mutex_found].nr_wlist] = pid;
		mlist[mutex_found].nr_wlist++;
		while(wake_process != pid && mlist[mutex_found].locked == true)
		{
			//sleep(2);
			sleep(1);
		}
		//plist[process_found].status = false;
		mlist[mutex_found].locked = true;
		return 0;
	}
	return -1;
}

int mtxunlock(int id)
{
	if(created_list)
	{
		for(int i=0; i<nr_mlist; i++)
		{
			if(mlist[i].id == id)
			{
				pid_t tmp;
				//printf("Nr wlist: %d\n",mlist[id].nr_wlist);
				if(mlist[id].nr_wlist > 0)
				{
					tmp = mlist[id].wlist[0];
				//printf("tmp:%d\n", tmp);
				//printf("Lungime %d\n", mlist[id].nr_wlist-1);
					for(int i=0; i < mlist[id].nr_wlist - 1; i++)
						mlist[id].wlist[i] = mlist[id].wlist[i+1];
					mlist[id].wlist = realloc(mlist[id].wlist, sizeof(pid_t)*(mlist[id].nr_wlist-1));
					mlist[id].nr_wlist--;
					wake_process = tmp;
				}
				mlist[i].locked = false;
				//printf("unlock %d\n", nr_mlist);
				//mtxgrant(id);
				return 0;
			}
		}
	}
	return -1;
}

int mtxgrant(int id)
{
		//printf("mtxgrant %d\n", created_list);
	if(created_list)
	{
		pid_t tmp;
			//printf("Nr wlist: %d\n",mlist[id].nr_wlist);
		if(mlist[id].nr_wlist > 0)
		{
			tmp = mlist[id].wlist[0];
				//printf("tmp:%d\n", tmp);
				//printf("Lungime %d\n", mlist[id].nr_wlist-1);
			for(int i=0; i < mlist[id].nr_wlist - 1; i++)
				mlist[id].wlist[i] = mlist[id].wlist[i+1];
			mlist[id].wlist = realloc(mlist[id].wlist, sizeof(pid_t)*(mlist[id].nr_wlist-1));
			mlist[id].nr_wlist--;
			wake_process = tmp;
		}
		return 0;
	}
	return -1;
}

int mtxlist(pid_t *list, int nlist)
{
	
}
