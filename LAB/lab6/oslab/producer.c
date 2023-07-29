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
int pos_in;			/* BUFFER POSITION ABOUT READ AND WRITE */

void Producer()
{

	int i;
	int *bf;
	int id;
	if((id=shmget("buffer",BUFFERSIZE+1,0)) == -1)
	{
		printf("申请失败\n");
		return;
	}
/*	printf("shmget is OK\n");*/
	
	bf = (int*)shmat(id,NULL,0);
	if(bf == -1)
	{
		printf("ERROR : bf == -1\n");
		return;
	}
/*	printf("shmat is OK\n");*/
	for(i=0;i<256;++i)
	{
		sem_wait(empty);
		sem_wait(mutex);
	/*	printf("empty:%dp %d\n",cnt,getpid());*/
	/* get offset of buffer and write the data */
/*		printf("%d %d\n",i,getpid()); 		*/
		bf[pos_in] = i;
/*		printf("value: %d\n",i);
		fflush(stdout);*/
		pos_in = (pos_in + 1) % BUFFERSIZE;		/* renew the buffer index */
	
		sem_wake(mutex);
		sem_wake(full);
	/*	printf("full:%dp\n %d",cnt,getpid());
		fflush(stdout);*/
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

	Producer();
	return 0;
}
