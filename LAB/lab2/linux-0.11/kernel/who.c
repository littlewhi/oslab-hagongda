//#include<string.h>
#include<errno.h>
#include<asm/segment.h>
#include<unistd.h>
//iam -> _syscall1(int, iam, const char*, name);
//whoami -> _syscall2(int,whoami,char*,name,unsigned,int,size)
//get_fs_byte(const char* c) get one byte from usersegment
//put_fs_byte(char res,const char* c) put res to c ->  
char kernel_name[30];
int namesize;
/*
 * iam
 * write the name in kernel
 * when length(name) > 23
 * errno = EINVAL
 * return size / -1
*/

int sys_iam(const char *name)
{
	//	return 128;
	int name_len = 0;
	while(get_fs_byte(name + name_len) != '\0')
		++name_len;														
//	printk("%d",name_len); 
	if(name_len > 23)
	{
//		printk("%d\n",name_len);
		errno = EINVAL;
		return -1;
	}

	int i = 0;
	for(;;++i)	//the '\0' as a sign
	{
		kernel_name[i] = get_fs_byte(name+i);
		if(kernel_name[i] == 0)
				break;
	}
	return namesize = i;
}

/*
 * whoami
 * copy the name in kernel to usersegment
 * return size
 * if size < len(name)
 * errno = EINVAL, return -1
*/
int sys_whoami(char* name, unsigned size)
{
	if(size < namesize)	//容量不足
	{
		errno = EINVAL;
		return -1;
	}	
	int i = 0;
	for(;kernel_name[i];++i)
		put_fs_byte(kernel_name[i],name+i);
	put_fs_byte(kernel_name[i],name+i);
	return i;
}
