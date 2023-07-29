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
_syscall3(int,shmget,const char*,key,size_t,size,int,shmflg);
_syscall3(void*,shmat,int,shmid,const void*,shmaddr,int,shmflg);

int empty,full,mutex;		/* SINGNAL VARIETY  */
int pos_out;			/* BUFFER POSITION ABOUT READ AND WRITE */

void Consumer()
{
	int i=0,res_read = 0,id/*,re*/;
	/* set the begining value of pos_out  in the end of buffer space */
	int* bf;
	if((id = shmget("buffer",BUFFERSIZE+1,0)) == -1)
	{
		printf("申请失败\n");
		return;
	}
	bf = shmat(id,NULL,0);
	for(i=0;i<256;++i)
	{
		sem_wait(full);
		sem_wait(mutex);
		
		res_read = bf[pos_out];
		pos_out = (pos_out + 1) % BUFFERSIZE;

		printf("%d : %d\n",getpid(),res_read);	/* consume the source */
		fflush(stdout);
		
		sem_wake(mutex);
		sem_wake(empty);
	
	}
}


int main()
{
	 if((empty = sem_open("empty",BUFFERSIZE)) < 0 ||
            (full = sem_open("full",0)) < 0  ||
            (mutex = sem_open("mutex",1)) < 0)
    {
        printf("ERROR!\n");
        return 0;
    }
	
	Consumer();	
	return 0;
}
