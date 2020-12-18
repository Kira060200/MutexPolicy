#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "mtx_header.h"

bool created_list;
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
int mtxopen(pid_t pid)
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
		if(!mlist[i].opened)
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
		for(int i=0; i<nr_plist; i++)
			if(plist[i].id == pid)
			{
				process_found = i;
				break;
			}
		if(process_found == -1)
			return -1;
		plist[process_found].status = true;
		if(mlist[mutex_found].locked == false)
		{
			mlist[mutex_found].locked = true;
			return 0;
		}
		mlist[mutex_found].wlist = realloc(mlist[mutex_found].wlist, sizeof(pid_t)*(mlist[mutex_found].nr_wlist++));
		mlist[mutex_found].wlist[mlist[mutex_found].nr_wlist-1] = pid;
		while(wake_process != pid);
		plist[process_found].status = false;
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
				mlist[i].locked = false;
				mtxgrant(id);
				return 0;
			}
		}
	}
	return -1;
}
