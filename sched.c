/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <errno.h>

/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS+2]
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

#if 0
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
#endif

extern struct list_head blocked;


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}

void init_idle (void)
{
	struct list_head *l = list_first(&freequeue);
	list_del(l);
	idle_task = list_entry(l,struct task_struct,list);
	idle_task-> PID = 0;
	if(allocate_DIR(idle_task) != 1) return -EAGAIN;
	idle_process = (union task_union*)idle_task;
	idle_process->stack[KERNEL_STACK_SIZE-1] = cpu_idle;
	idle_process->stack[KERNEL_STACK_SIZE-2] = 0;
	idle_task->kernel_esp = idle_process->stack[KERNEL_STACK_SIZE-2];

}

void init_task1(void)
{
	struct list_head *l = list_first(&freequeue);
	list_del(l);
	init_task = list_entry(l,struct task_struct,list);
	init_task-> PID = 1;
	if(allocate_DIR(init_task) != 1) return -EAGAIN;
	init_process = (union task_union*)init_task;
	init_task->kernel_esp = init_process->stack[KERNEL_STACK_SIZE-1];
	set_user_pages(init_task);
	set_TTS_esp0(init_process->stack[KERNEL_STACK_SIZE]);
	set_cr3(init_task->dir_pages_baseAddr);		

}

void switch_to_idle() {

	task_switch(idle_task);

}


void init_sched(){
	INIT_LIST_HEAD(&freequeue);
	INIT_LIST_HEAD(&readyqueue);
	for(int i = 0; i < NR_TASKS; ++i) {
		task[i].task.PID = -1;
		list_add(&task[i].task.list,&freequeue);
	}
}

void task_switch(union task_union *new) {

	__asm__ __volatile__(
		"pushl %esi;"
		"pushl %edi;"
		"pushl %ebx;"
	);

	inner_task_switch(new);

	__asm__ __volatile__(
		"popl %ebx;"
		"popl %edi;"
		"popl %esi;"
	);

}

void inner_task_switch(union task_union * t) {
	
	int ebp, new_esp, esp;
	struct task_struct *current_task;

	set_TTS_esp0((int)&(t->stack[KERNEL_STACK_SIZE]));
	set_cr3(t->task.dir_pages_baseAddr);

	__asm__ __volatile__(
		"movl %%ebp,%0;"
		:"=g"(ebp)
	);


	current_task = current();
	current_task->kernel_esp = ebp;
	new_esp = t->task.kernel_esp;

	__asm__ __volatile__(
		"movl %0, %%esp;"
		"popl %%ebp;"
		"ret;"
		::"g"(new_esp)
	);
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

