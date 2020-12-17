#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "mtx_header.h"

bool created_list;
int nlist = 0;
struct process
{
	pid_t id;
	bool status; //1 for waiting, else 0
	int * h_mlist;
	int h_mlist_size ;
};
struct mutex
{
	int id;
	bool opened;
	bool locked;
	//pid_t * plist;
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
		mlist[0].id = nlist;
		nlist = 1;
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
	for (int i = 0; i<nlist; i++)
	{
		if(!mlist[i].opened)
		{
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
		mlist = realloc(mlist, sizeof(struct mutex)*(nlist+1));
		mlist[nlist].opened = true;
		mlist[nlist].locked = false;
		mlist[nlist].id = nlist;
		p->h_mlist = realloc(p->h_mlist, sizeof(int*)*(p->h_mlist_size + 1));
		p->h_mlist[p->h_mlist_size] = mlist[nlist].id;
		p->h_mlist_size ++;
		nlist++;
	}
	return 0;
}
