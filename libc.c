/*
 * libc.c 
 */

#include <libc.h>
#include <types.h>
#include <errno.h>

int errno;

void perror() {

	switch(errno) {
	
		case EINVAL:
			write(1,"parametro invalido\n",19);
			break;
		case ENOSYS:
			write(1,"funcion no implementada\n",24);
	}
}

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

int write(int fd , char * buffer, int size) 
{

	int result;
    	__asm__ __volatile__ (

	
		//Trap Interrupt
		
		"int $0x80;" //genero la interrupcio 0x80
	
		//Guardem el resultat en la variable e

		:"=a" (result)
		: "a" (4), "b" (fd), "c" (buffer), "d" (size)
        );

    if(result < 0){
	errno = -result;
	return -1;
    }
    return result;
}

int gettime() 
{
	int result;
    	__asm__ __volatile__(
	
		//Trap Interrupt
		
		"int $0x80;" //genero la interrupcio 0x80
	
		//Guardem el resultat en la variable result

		:"=a" (result)
		:"a"(10)
	);

	if(result < 0) 
	{
		errno = -result;
		return -1;	
	}

	return result;

}

int getpid() {

	int result;

    	__asm__ __volatile__ (

       
		//Trap Interrupt
		
		"int $0x80" //genero la interrupcio 0x80

		:"=a"(result)	
		:"a"(20)
		
	);

	return result;

}

int fork() {

	int pid;

    	__asm__ __volatile__ (

       
		//Trap Interrupt
		
		"int $0x80" //genero la interrupcio 0x80

		:"=a"(pid)	
		:"a"(2)
		
	);

	return pid;

}


