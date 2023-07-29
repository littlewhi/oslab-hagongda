#define __LIBRARY__
#include <linux/kernel.h>
#include <asm/segment.h>
#include <linux/sched.h>
/*#include <string.h>*/
#include <asm/system.h>
#include <unistd.h>
int sign = 0;

int sys_sem_open(const char *name,int value)
{
	char kernelname[50] = "";
	int i = 0;
	int empty = -1;
	while( kernelname[i] = get_fs_byte(name+i))
			++i;
	
	if(strlen(kernelname) >= SEMNAME)//名字长度超出规范
		return -1;

	for(i=0;i<SEMCAP;++i)
	{
		if(sem_table[i].name[0] == 0)
			empty = i;
		else if(strcmp(kernelname,sem_table[i].name) == 0) //找到信号量
			return i;
	}
	
	if(empty == -1)//信号量表已满
		return -1;
	
	strcpy(sem_table[empty].name,kernelname);
/*	printk("%d\n",value);*/
	sem_table[empty].val = value;
	sem_table[empty].head = 0;
	sem_table[empty].tail = 0;
	return empty;
}

int sys_sem_wait(int i)
{
	//P 原子操作 test
	if(i >= SEMCAP || i < 0)
		return -1;

	cli();
/*	printk("OKa1\n");
	printk("%d %d\n",i,sem_table[i].val);
*/	--(sem_table[i].val);
	if(sem_table[i].val < 0)
	{
/*		printk("OKa2\n");*/
		int head = sem_table[i].head,tail=sem_table[i].tail;
		if((tail+1)%QLEN == head)//等待队列满了
		{
			sti();
			return -1;
		}
		sem_table[i].queue[tail] = current;//push in wait queue
		current->state = TASK_UNINTERRUPTIBLE;
		sem_table[i].tail = (tail+1)%QLEN;
		schedule();
	}

	sti();

	return sem_table[i].val;
}

int sys_sem_wake(int i)
{
		//增加
	if(i >= SEMCAP || i < 0)
		return -1;

	cli();
/*	printk("OKay\n");*/
	++(sem_table[i].val);
	if(sem_table[i].val <= 0 )
	{
	/*	printk("Okay\n");*/
		int head = sem_table[i].head,tail=sem_table[i].tail;
		if(head != tail)//queue is not empty
		{
			//wake up the front
			sem_table[i].queue[head]->state = TASK_RUNNING;
			sem_table[i].head = (head+1)%QLEN;
			schedule();
		}
	}

	sti();
	return sem_table[i].val;

}

int sys_sem_close(const char *name)
{
	char kernelname[50] = "";
	int i = 0;
	while(kernelname[i++] = get_fs_byte(name))
			;
	
	if(strlen(kernelname) >= SEMNAME)
		return -1;
	
	for(i=0;i<SEMCAP;++i)
	{
		if(strcmp(kernelname,sem_table[i].name)==0)
			break;
	}
	
	if(i == SEMCAP)
	{
		printk("Can not find the semaphore : %s\n",kernelname);
		return -1;
	}
	sem_table[i].name[0] = 0;
	sem_table[i].head = sem_table[i].tail = 0;
	sem_table[i].val = 0;
	return 0;
}

int sys_getSign()
{
	return sign;
}

int sys_setSign()
{
	cli();
	sign = 1;
	sti();
	return 1;
}

int sys_resetSign()
{
	cli();
	sign = 0;
	sti();
	return 0;
}
