/*
 * libc.c 
 */

#include <libc.h>
#include <types.h>
#include <errno.h>

int errno;

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
       
		"pushl %%ebx ;"
		"movl 8(%%ebp),%%ebx ;"
		"movl 12(%%ebp),%%ecx ;"
		"movl 16(%%ebp),%%edx;"
		"movl 20(%%ebp),%%esi;"
		"movl 24(%%ebp),%%edi;"
		
		//Put the identifier of the system call in the eax register (number 4 for write)

		"movl $4, %%eax;" 
	
		//Trap Interrupt
		
		"int $0x80;" //genero la interrupcio 0x80
		"popl %%ebx;" 
		"movl %%eax, %0"
	
		//Guardem el resultat en la variable e

		:"=g" (result)
        );

    if(result < 0){
        return -1;
    }
    return result;
}


