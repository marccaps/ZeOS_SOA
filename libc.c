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

		/*
			Parameter  passing:  ZeOS  implements  the  parameter  passing  from  user  mode  to  system
			mode through the CPU registers, as occurs in Linux. The parameters of the stack must be
			copied to the registers ebx, ecx, edx, esi, edi. The correct order is the first parameter (on the
			left) in ebx, the second parameter in ecx, etc.
		*/
       
	
		//Trap Interrupt
		
		"int $0x80;" //genero la interrupcio 0x80
	
		//Guardem el resultat en la variable e

		:"=a" (result)
		: "a" (4), "b" (fd), "c" (buffer), "d" (size)
        );

    if(result < 0){
        return errno;
	errno = result;
	perror();
    }
    return result;
}

int gettime() 
{
	int result;
    	__asm__ __volatile__(
	
		//Trap Interrupt
		
		"int $0x80;" //genero la interrupcio 0x80
		"movl $10,%%eax"
	
		//Guardem el resultat en la variable e

		:"=g" (result)
	);

	if(result < 0) 
	{
		errno = result;
		perror();
		return errno;	
	}

	return result;

}


