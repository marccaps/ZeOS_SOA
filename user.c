#include <libc.h>


char buff[24];
char buffAux[512];

int pid;


void prueba_estres(int n) {
	
	int a = 10;

	for(int i = 0; i < n; ++i) {

		a = a*2;

	}
}

/*Esta funcion realiza mucho calculo , pero no se bloquea , ya que no hace instrucciones de I/O*/

void workload1() {

	int pid1 = fork();
	if(pid1 = 0) {
		int pid2 = fork();
		if(pid2 = 0) {
			prueba_estres(2000000);
			print_status();
			exit();
		}
		else {
			prueba_estres(2000000);
			print_status();
			exit();
		}
	}
	else {
		prueba_estres(2000000);
		print_status();
	}
}

void workload2(){
    
	int pid1,pid2,f,r,i;
	char buffer[32];
	pid = fork();

	if(pid1 == 0){
		pid2 = fork();
		if(pid2 == 0){
		    r = read(0,&buffer, 150);
		    prueba_estres(300000);
		    r = read(0,&buffer, 150);
		    print_status();
		    exit();
		}
		else{
			r = read(0,&buffer, 150);
			prueba_estres(300000);
			r = read(0,&buffer, 150);
			print_status(); 
			exit();
		}
	}
	else{
		r = read(0,&buffer,1);
		r = read(0,&buffer, 150);
		prueba_estres(300000);
		r = read(0,&buffer, 150);
		print_status();
	}
}

void workload3(){
	
    char buff[32];
    int pid,pid_f,f,r;
    pid = fork();
    if(pid == 0){
        pid_f = fork();
        if(pid_f == 0){
            f = read(0, &buff,50);
            prueba_estres(600000);
            print_status();
            exit();
        }
        else{
            prueba_estres(1000000);
            r = read(0,&buff,200);
            prueba_estres(2000000);
            r = read(0,&buff,100);
            print_status();
            exit();
        }
    }
    else{
        prueba_estres(100000000);
        prueba_estres(100000000);
        print_status();
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

void print_status()
{
	struct stats st;
	get_stats(getpid(),&st);
	itoa(st.user_ticks, buff);
	itoa(getpid(),buffAux);
	write(1,buffAux,strlen(buffAux));
	write(1,"user_ticks: ",strlen("user_ticks: "));
	write(1,buff,strlen(buff));
	write(1,"\n",1);

	itoa(st.system_ticks, buff);
	itoa(getpid(),buffAux);
	write(1,buffAux,strlen(buffAux));
	write(1,"system_ticks: ",strlen("system_ticks: "));
	write(1,buff,strlen(buff));
	write(1,"\n",1);

	itoa(st.blocked_ticks, buff);
	itoa(getpid(),buffAux);
	write(1,buffAux,strlen(buffAux));
	write(1,"blocked_ticks: ",strlen("blocked_ticks: "));
	write(1,buff,strlen(buff));
	write(1,"\n",1);

	itoa(st.ready_ticks, buff);
	itoa(getpid(),buffAux);
	write(1,buffAux,strlen(buffAux));
	write(1,"ready_ticks: ",strlen("ready_ticks: "));
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
		set_sched_policy(1);

		exec_workload(3);	


	}
}
