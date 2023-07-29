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
_syscall1(int,write_buffer,int,i);

int empty,full,mutex;		/* SINGNAL VARIETY  */
int pos_in,pos_out;			/* BUFFER POSITION ABOUT READ AND WRITE */
int fd;									/* THE MARK OF BUFFER FILE  */

void Producer()
{

	int i;

	for(i=0;i<1024;++i)
	{
		sem_wait(empty);
		sem_wait(mutex);
		/* get offset of buffer and write the data */
		lseek(fd,pos_in*sizeof(int),SEEK_SET);
		write(fd,(char*)&i,sizeof(int));
		
		pos_in = (pos_in + 1) % BUFFERSIZE;		/* renew the buffer index */
	
		sem_wake(mutex);
		sem_wake(full);
	}
	
}


void Consumer()
{
	int i,res_read;
	
	for(i=0;i<256;++i)
	{
		sem_wait(full);
		sem_wait(mutex);

		/* get offset of buffer and read the data */
		lseek(fd,pos_out*sizeof(int),SEEK_SET);
		read(fd,(char*)&res_read,sizeof(int));

		pos_out = (pos_out + 1) % BUFFERSIZE;	/* renew buffer index position */

		printf("%d : %d",getpid(),res_read);	/* consume the source */

		sem_wake(mutex);
		sem_wake(empty);
	}
}


int main()
{
	int f1 = fork();
	int f2 = fork();

	if(f1 && f2)
	{
		/* get signal variety */ 
		if((empty = sem_open("empty")) < 0 ||
				(full = sem_open("full")) < 0	 ||
				(mutex = sem_open("mutex")) < 0)
		{
			printf("ERROR!\n");
			return 0;
		}

		pos_out = pos_in = 0;			/* put beginning value in */
		fd = open("buffer.txt");	/* get buffer file */
		Producer();
	}
	else
	{
		Consumer();
	}
	return 0;
}
