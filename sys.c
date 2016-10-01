/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
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

	if(check_fd(fd,ESCRIPTURA) != 0) {
		return -1;
	}
	else {
		if(buffer != NULL) 
		{
			if(size >= 0) 
			{
				//TODO:Enviar a escribir
				int written = 0;
				char container[size];
				if(copy_from_user(buffer,&container,size) >= 0) 
				{
					sys_write_console(&container,size);

				}
				else return -1;
				return size;
			}
			else return -1;					
		}
		else return -1;
	} 
}

void sys_exit()
{  
}





