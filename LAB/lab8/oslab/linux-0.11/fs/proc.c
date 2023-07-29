#include<linux/kernel.h>
#include<sys/types.h>
#include <stdarg.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <asm/segment.h>

const char titlep[] = "PID\tSTATE\tFATHER\tCOUNTER\tSTART_TIME\n";
char* temp = 0;
int num_ps = 0;
const size_t len = sizeof(titlep);

int sprintf(char *buf, const char *fmt, ...)
{
    va_list args; int i;
    va_start(args, fmt);
    i=vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}
int psinfo(char *buf,int count,int* pos)
{
	/*
	 * pos is the position in file 
	 * if pos == 65 reset everything
	 * if pos == 0 first read title
	 * else
	 *	read task
	*/
	int i,cnt=0,lofone;
	struct task_struct **k;
	char test[100];
	if(num_ps == 64)
	{
		num_ps = 0;
		return 0;
	}
	
	temp = (char*)malloc(count);

	if(num_ps == 0)
	{
		cnt += sprintf(temp,titlep);
	}
	
	lofone = sprintf(test,"%3d\t%5d\t%6d\t%7d\t%10d\n",1,2,3,4,5);

	for(k = (&FIRST_TASK) + num_ps; k <= &LAST_TASK; ++k)
	{
		if(*k && count - cnt > lofone)
		{
			cnt += sprintf(temp+cnt,"%3d\t%5d\t%6d\t%7d\t%10d\n",(*k)->pid,(*k)->state,(*k)->father,(*k)->counter,(*k)->start_time);		
		}
		if(count - cnt <= lofone)
			break;
		
		++num_ps;
	}	

	i = 0;
	for(i;i<cnt;++i)
	{
		put_fs_byte(temp[i],buf+i);
		++(*pos);
	}
	free(temp);
	
	return cnt;

}

static int getBit1(unsigned char n)
{
	int ret = 0;
	int i = 0;
	for(i;i<(sizeof(n)<<3);++i)
	{
		if(n&(1<<i))
			++ret;
	}
	return ret;
}

int hdinfo(struct super_block *sb,char *buf,int count, int *pos)
{
	if(*pos != 0)
		return *pos = 0;
	//only print three information, so I don't do more work for other circumstances.
	char a[50] = { 0 };
	struct buffer_head *bh = NULL;
	int total_block = sb->s_nzones;
	int i,total_used=0,total_free=0,cnt=0,strl;
	char* p = NULL;
//	//printk("hdinfor\n");
	for(i=0;i<NR_SUPER;++i)
	{
		if(bh = sb->s_zmap[i])
		{
			int j=0,num;
			p = bh->b_data;
			for(j=0;j<BLOCK_SIZE;++j)
			{
				num = getBit1(p[i]);
				total_used += num;
				total_free += (sizeof(char)<<3) - num;
			}
		}
	}

	total_free -= total_used + total_free - total_block;

	i=0;
	strl = sprintf(a,"TOTAL BLOCKS:\t%5d\n",total_block);
	while(a[i] != 0)
	{
		put_fs_byte(a[i],buf+i);
		++i;
	}
	cnt += strl;
	
	strl = sprintf(a,"USED BLOCKS:\t%5d\n",total_used);
	i=0;
	while(a[i] != 0)
	{
		put_fs_byte(a[i],buf+i+cnt);
		++i;
	}
	cnt += strl;
	
	strl = sprintf(a,"FREE BLOCKS:\t%5d\n",total_free);
	i=0;
	while(a[i] != 0)
	{
		put_fs_byte(a[i],buf+i+cnt);
		++i;
	}
	cnt += strl;
	//printk("4");
	*pos = 1;
	return cnt;
}

int proc_read(struct m_inode* inode,char *buf,int count,int* pos)
{
	dev_t dev = inode->i_zone[0];
	//printk("proc_read\n");
	if(dev == 0)
	{
		return psinfo(buf,count,pos);
	}
	if(dev == 1)
	{
		//printk("hd\n");
		struct super_block *sb = get_super(inode->i_dev);
		return hdinfo(sb,buf,count,pos);
	}
	return -1;
}
