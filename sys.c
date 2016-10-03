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
  int PID=-1;

  // creates the child process
  
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





