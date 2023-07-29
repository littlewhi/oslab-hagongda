#include <asm/segment.h>
#include <unistd.h>
#include <signal.h>

#include <asm/system.h>

#include <linux/sched.h>
#include <linux/head.h>
#include <linux/kernel.h>
#include <linux/mm.h>
/* This function is to get a free page for key
 * key is the id of memory needed
 * if key exists, then return.
 * size is the size of memory needed
 * shmflg can be ignored
 * return and id in system management
 * if size is oversize will return -1
 * or system can not find free memory space
 * */
int sys_shmget(const char* key, size_t size, int shmflg)
{
	char kernelname[50];
	int i = 0;
	while(kernelname[i] = get_fs_byte(key+i))
		++i;
//	printk("get name is OK\n");
	if(i >= 20)
	{
		printk("name is too long.\n");
		return -1;
	}

	for(i=0;i<SHM_TABLE_SIZE;++i)
	{
		if(shm_table[i].state && strcmp(kernelname,shm_table[i].name) == 0)
			return i;
	}

	for(i=0;i<SHM_TABLE_SIZE;++i)
	{
		if(shm_table[i].state == 0)
			break;
	}
//	printk("find state is OK\n");
	if(i == SHM_TABLE_SIZE)
	{
		printk("cannot find free shared memory\n");
		return -1;
	}

	strcpy(shm_table[i].name,kernelname);
	shm_table[i].page = get_free_page();
//	printk("get free page is OK\n");
	shm_table[i].size = size;
	shm_table[i].state = 1;
	return i;
}

/* shmid is the id returned by shmget()
 * connect the virtual memory address with shared_memory page space.
 * */
void *sys_shmat(int shmid, const void *shmaddr, int shmflg)
{
	unsigned long* base;
	if(shmid > SHM_TABLE_SIZE || shmid < 0)
	{
		printk("shmid is wrong number\n");
		return (void*)0;
	}
/*
	base = (unsigned long*)get_base(current->ldt[2]);
*/
	if(shm_table[shmid].state == 0)
	{
		printk("ERROR\n");
		return 0;
	}
	base = (unsigned long*)(current->brk + current->start_code);	
	put_page(shm_table[shmid].page,(unsigned long)base);

	shm_table[shmid].attach += 1;
	shm_table[shmid].addr = (unsigned long)current->brk;
	
	return (void*)current->brk;
}
