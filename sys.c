/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <libc.h>

#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1
#define MAX_SIZE 512

int MAX_PID = 2;

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -EACCES; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -ENOSYS; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{

	/*Inicializaciones*/

	int PID=-1;
	int free_frames[];
	struct task_struct *child_struct ,*parent_struct;
	struct task_union *child_union,*parent_union;
	page_table_entry *parent_PT,*child_PT;



	/*a*/

	if(list_empty(&freequeue)!= 0) {

		return -EAGAIN;

	}

	/*b*/

	struct list_head *l = list_first(&freequeue);
	list_del(l);
	parent_struct = current();
	parent_union = (union task_union*) parent_struct;
	child_struct = list_entry(l,struct task_struct,list);
	copy_data(parent_struct,child_struct,KERNEL_STACK_SIZE*4);

	/*c*/
	if(allocate_DIR(child_struct) != 1) return -EAGAIN;

	/*d*/
	for(int i = 0; i < NUM_PAG_DATA; ++i) {

		free_frames[i] = alloc_frames();
		//Surge un error y por lo tanto dejamos tal y como estaba todo
		if(free_frames[i] == -1) {
			while(i >= 0) free_frame(free_frames[--i]);
			list_add(child_struct->list,&freequeue);	
			return -ENOMEM;
		}
	}

	/*e*/
	child_PT = get_PT(child_struct);
	for(int i = 0; i < NUM_PAG_KERNEL + NUM_PAG_CODE;++i) {

		set_ss_pag(child_PT,i,get_frame(parent_PT,i));	

	}

	for(int i = PAG_LOG_INIT_DATA; i < PAG_LOG_INIT_DATA+NUM_PAG_DATA;++i) {
	
		set_ss_pag(child_PT,i,free_frame[i-PAG_LOG_INIT_DATA]);
		set_ss_pag(parent_PT,i+20,free_frame[i-PAG_LOG_INIT_DATA]);

	
	}

	child_struct->dir_pages_baseAddr = get_PT(child_struct);


	/*f*/

	PID = MAX_PID;
	MAX_PID = PID+1;
	child_task->PID = PID;
	
	
		
	





	/*i*/

	list_add_tail(&child_task->list,&readyqueue);

	/*j*/
  
	return PID;
}

int sys_write(int fd, char *  buffer , int size) 
{
	int check_fd_result = check_fd(fd,ESCRIPTURA);
	if(check_fd_result != 0) {
		return check_fd_result;
	}
	else {
		if(buffer != NULL) 
		{
			int written = 0;
			int sumatori_written = 0;
			char container[MAX_SIZE];
			while(size > MAX_SIZE) 
			{				
				if(copy_from_user(buffer+written,container,MAX_SIZE) >= 0) 
				{
					written = sys_write_console(container,MAX_SIZE);
					sumatori_written += written;
					size -= written;
					
				}
				else return -EINVAL;
			}
			if(size >= 0) 
			{
				if(copy_from_user(buffer+written,container,size) >= 0) 
				{
					written = sys_write_console(container,size);
					sumatori_written += written;
					size -= written;
					
				}
				else return -EINVAL;
				return sumatori_written;
			}
			else return -EINVAL;
		}
		else return -EFAULT;
	} 
}

void sys_exit()
{  
}

int sys_gettime() {

}





