#include <libc.h>


char buff[24];

int pid;


void prueba_estres(int n) {
	
	int a = 10;

	for(int i = 0; i < n; ++i) {

		a = a*2+1;

	}
}

/*Esta funcion realiza mucho calculo , pero no se bloquea , ya que no hace instrucciones de I/O*/

void workload1() {

	int pid1 = fork();
	if(pid1 = 0) {
		int pid2 = fork();
		if(pid2 = 0) {
			prueba_estres(2000000);
			exit();
		}
		else {
			prueba_estres(2000000);
			exit();
		}
	}
	else {
		prueba_estres(2000000);
	}
}

void workload2(){
    
	int pid1,pid2,f,r,i;
	char buffer[32];
	pid = fork();

	if(pid1 == 0){
		pid2 = fork();
		if(pid2 == 0){
		    r = read(0,&buffer, 1500);
		    prueba_estres(300000);
		    r = read(0,&buffer, 1500);
		    print_act();
		    exit();
		}
		else{
			r = read(0,&buffer, 1500);
			prueba_estres(300000);
			r = read(0,&buffer, 1500);
			print_act(); 
			exit();
		}
	}
	else{
		r = read(0,&buffer,1);
		r = read(0,&buffer, 1500);
		prueba_estres(300000);
		r = read(0,&buffer, 1500);
		print_act();
	}
}

void workload3(){
	
	char buffer[32];
	int pid1,pid2,f,r;

	pid1 = fork();

	if(pid1 == 0){
		pid2 = fork();
		if(pid2 == 0){
			f = read(0, &buff,50);
			prueba_estres(600000);
			print_act();
			exit();
		}
		else{
			prueba_estres(1000000);
			r = read(0,&buff,200);
			prueba_estres(2000000);
			r = read(0,&buff,100);
			print_act();
			exit();
		}
	}
	else{
		prueba_estres(100000000);
		prueba_estres(100000000);
		print_act();
		exit();
	}
}

void exec_workload(int n)
{
    if(n == 1)
        workload1();
    else if (n == 2)
        workload2();
    else if(n == 3)
        workload3();
}

void print_act()
{
	int pid,e;
	struct stats st,st2;
	pid = getpid();
	write(1,"\nPID: ",5);
	print_int(pid);
	e = get_stats(pid,&st);
	if(e != 0){
		write(1,"error\n",6);
		return;
	}
	print(&st);
	if(pid == 1){
		e = get_stats(0,&st2);
		write(1,"IDLE:\n",6);
		print(&st2);
	}
}

void print(struct stats *st)
{
	int n, t;
	char b[32];
	t = 0;
	n = st->user_ticks;
	write(1,"user:\n",6);
	print_int(n);
	t += n;
	write(1,"blocked:\n",9);
	n = st->blocked_ticks;
	t += n;
	print_int(n);

	write(1,"ready:\n",7);
	n = st->ready_ticks;
	t += n;
	print_int(n);

	write(1,"system:\n",8);
	n = st->system_ticks;
	t += n;
	print_int(n);

	write(1,"total:\n",7);
	print_int(t);

}

void print_int(int a) {

	char buff[64];
	itoa(a,buff);
	write(1,buff,strlen(buff));
	write(1,"\n",1);
}



int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

	while(1) 
	{ 

		exec_workload(1);	

	}
}
