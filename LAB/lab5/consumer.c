#define __LIBRARY__
#include<unistd.h>
#include<asm/segment.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>
_syscall2(int,sem_open,const char*,name,int,value);
_syscall1(int,sem_close,const char*, name);
_syscall1(int,sem_wait,int,i);
_syscall1(int,sem_wake,int,i);
_syscall0(int,read_buffer);

int empty;
int full;
int mutex;
#define BUFFERSIZE 10
/*int buffer[BUFFERSIZE];*/

void Consumer()
{
	unsigned pid;
	int num;
	int i;
	empty = sem_open("empty",BUFFERSIZE);
	full = sem_open("full",0);
	mutex = sem_open("mutex",1);
/*	printf("%d\n",empty);	*/
	for(i=0;i<52;++i)
	{
		sem_wait(full);/*是否有满的的缓冲区*/
		sem_wait(mutex);/*互斥信号量*/
	
		pid = getpid();
		num = read_buffer();
		printf("%d: %d\n",pid,num);
		fflush(stdout);

	/*	write(fd,output,cnt);
		write(fd,output,cnt);*/
		sem_wake(mutex);
		sem_wake(empty);
		}	
}



int main(void)
{
	Consumer();
	return 0;
}
