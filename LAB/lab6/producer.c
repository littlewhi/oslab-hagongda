#define __LIBRARY__
#include<unistd.h>
#include<asm/segment.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>
#define BUFFERSIZE 10
_syscall2(int,sem_open,const char*,name,int,value);
_syscall1(int,sem_close,const char*, name);
_syscall1(int,sem_wait,int,i);
_syscall1(int,sem_wake,int,i);
_syscall3(int,shmget,const char*, key, size_t, size, int, shmflg);
_syscall3(void*,shmat,int,shmid,const void*,shmaddr,int, shmflg);

int empty,full,mutex;		/* SINGNAL VARIETY  */
int pos_in;			/* BUFFER POSITION ABOUT READ */

void Producer()
{

	int i;
	int *bf;
	int id;

	if((id=shmget("buffer",BUFFERSIZE+1,0)) == -1)	/* get page memory */
	{
		printf("申请失败\n");
		return;
	}
	
	bf = (int*)shmat(id,NULL,0);					/* set connection between linear address and page */
	if(bf == -1)
	{
		printf("ERROR : bf == -1\n");
		return;
	}

	for(i=0;i<256;++i)
	{
		sem_wait(empty);
		sem_wait(mutex);

		bf[pos_in] = i;								/* produce data */
		pos_in = (pos_in + 1) % BUFFERSIZE;			/* renew the position in buffer */
	
		sem_wake(mutex);
		sem_wake(full);
	
	}
	
}

int main()
{

	/* get signal variety */ 
	if((empty = sem_open("empty",BUFFERSIZE)) < 0 ||
			(full = sem_open("full",0)) < 0	 ||
			(mutex = sem_open("mutex",1)) < 0)
	{
		printf("ERROR!\n");
		return 0;
	}
	/* come into produecer */
	Producer();
	return 0;
}
