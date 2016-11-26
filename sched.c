/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <stats.h>
#include <utils.h>
#include <schedperf.h>

/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS+2]
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

struct list_head freequeue; /*Free queue*/
struct list_head readyqueue; /*Ready queue*/
struct task_struct *idle_task; /*idle task*/
struct task_struct *init_task; // TEST

struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}

extern struct list_head blocked;

#define DEFAULT_QUANTUM 10
int remaining_quantum = 0;


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

void init_stats(struct stats *st)
{
	st->user_ticks = 0;
	st->system_ticks = 0;
	st->blocked_ticks = 0;
	st->ready_ticks = 0;
	st->elapsed_total_ticks = get_ticks();
	st->total_trans = 0;
	st->remaining_ticks = get_ticks();
}

void init_idle (void)
{
	struct list_head *firstFree = list_first(&freequeue);
	list_del(firstFree); // Esborrem l'element de la cua que volem utilitzar
	idle_task = list_head_to_task_struct(firstFree);

	union task_union * idle_task_union = (union task_union *)idle_task;

	idle_task_union->task.PID = 0;
	idle_task_union->task.quantum = DEFAULT_QUANTUM;
	idle_task_union->task.process_state = ST_RUN;
	init_stats(&idle_task_union->task.process_stats);
	allocate_DIR(&idle_task_union->task);
	idle_task_union->stack[KERNEL_STACK_SIZE-1] = (unsigned long)cpu_idle; //(unsigned long)&cpu_idle;
	idle_task_union->stack[KERNEL_STACK_SIZE-2] = 0; /*ebp*/
	idle_task_union->task.kernel_esp = (unsigned long)&(idle_task_union->stack[KERNEL_STACK_SIZE-2]);

}

void init_task1(void)
{	
  struct list_head *firstFree = list_first(&freequeue);
  list_del(firstFree);
  //struct task_struct *init_task1 = list_head_to_task_struct(firstFree);
	init_task = list_head_to_task_struct(firstFree);
  
  //union task_union * init_task1_union = (union task_union *)init_task1;
	union task_union * init_task1_union = (union task_union *)init_task;
  
  init_task1_union->task.PID = 1;
	init_task1_union->task.quantum = DEFAULT_QUANTUM;
	init_task1_union->task.process_state = ST_RUN;
	remaining_quantum = DEFAULT_QUANTUM;
	init_stats(&init_task1_union->task.process_stats);
  allocate_DIR(&init_task1_union->task);
  set_user_pages(&init_task1_union->task);

	tss.esp0 = (unsigned long)&init_task1_union->stack[KERNEL_STACK_SIZE];
	
	page_table_entry * dir_task1 = get_DIR(&init_task1_union->task);
	set_cr3(dir_task1);
}


void init_sched(){
  /*FREE QUEUE*/
  INIT_LIST_HEAD( &freequeue );
  
  int i;
  for (i = 0; i < NR_TASKS; ++i) {
    list_add_tail( &(task[i].task.list), &freequeue );
  }
  
  /*READY QUEUE*/
  INIT_LIST_HEAD( &readyqueue );
	
	init_sched_policy();
  
}

void inner_task_switch(union task_union *new) {

	tss.esp0 = (unsigned long)&new->stack[KERNEL_STACK_SIZE];	
	
	page_table_entry * dir_task_switch = get_DIR(&new->task);
	set_cr3(dir_task_switch);

	struct task_struct * current_TS = current();
	unsigned long new_kernel_esp = new->task.kernel_esp;
	
	asm("mov %%ebp,%0;"
			"mov %1,%%esp;"
			"popl %%ebp;"
			"ret;"
			:
			: "g" (current_TS->kernel_esp), "g" (new_kernel_esp)
	);
	
	// Primer g, usar m o g

}

void task_switch(union task_union *new) {
	//printk("task_switch");
	/*SAVE esi, edi and ebx*/
	asm("pushl %esi;"
			"pushl %edi;"
			"pushl %ebx;"
	);
	
	inner_task_switch(new);	

	/*RESTORE esi, edi and ebx*/
	asm("popl %ebx;"
			"popl %edi;"
			"popl %esi;"
	);
}

/*Scheduling policy*/

int get_quantum(struct task_struct *t) {
	return t->quantum;
}

void set_quantum(struct task_struct *t, int new_quantum) {
	t->quantum = new_quantum;
}

void update_sched_data_rr(void) {
	--remaining_quantum;
}

int needs_sched_rr(void) {
	if ((remaining_quantum <= 0)&&(!list_empty(&readyqueue))) return 1;
	if (remaining_quantum <= 0) remaining_quantum = get_quantum(current());
	return 0;
}

void update_process_state_rr(struct task_struct *t, struct list_head *dst_queue) {
	if (t->process_state!=ST_RUN) list_del(&(t->list));
	if (dst_queue!=NULL)
	{
	  list_add_tail(&(t->list), dst_queue);
	  if (dst_queue!=&readyqueue) t->process_state=ST_BLOCKED;
	  else
	  {
	    if (!(t->process_state==ST_BLOCKED))update_stats(&(t->process_stats.system_ticks), &(t->process_stats.elapsed_total_ticks),0);
	    else {
	      update_stats(&(t->process_stats.system_ticks), &(t->process_stats.elapsed_total_ticks),1);
	    }
	    t->process_state=ST_READY;
	  }
	}
	else t->process_state=ST_RUN;
}

void sched_next_rr(void) {
	struct task_struct *t;
	if (!list_empty(&readyqueue)) { /* As list is not empty, execute next process */
		struct list_head *next_process;
		next_process = list_first(&readyqueue);
		list_del(next_process);
		t = list_head_to_task_struct(next_process);
	}
	else { /* If list is empty, execute idle_task */
		t = idle_task;
	}
	
	/* Change the state to running */
	t->process_state=ST_RUN;

	remaining_quantum = get_quantum(t);
	update_stats(&(current()->process_stats.system_ticks), &(current()->process_stats.elapsed_total_ticks),0);
	update_stats(&(t->process_stats.ready_ticks), &(t->process_stats.elapsed_total_ticks),0);
	t->process_stats.total_trans++;

	/* Change to next process */
	task_switch((union task_union*)t);
}

void schedule() {
	/* Update the number of ticks */
	update_sched_data();
	if (needs_sched()) { /* Necessity to change the current process */
	  
		/* Update the state of a process */
		update_process_state(current(),&readyqueue);
		
		/* Select next process */
		sched_next();
	}
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

struct stats * get_task_stats(struct task_struct *t) {
  return (struct stats *)(&(t->process_stats));
}

struct list_head *get_task_list(struct task_struct *t) {
  return (struct list_head *)&(t->list);
}

void block_process(struct list_head *block_queue) {
  //printk("BLOCK");
  struct task_struct *t = current();
  struct stats *st = get_task_stats(t);
  
  update_process_state(t,block_queue);
  st->system_ticks = get_ticks()-(st->elapsed_total_ticks);
  st->elapsed_total_ticks = get_ticks();
  sched_next();
}

void unblock_process(struct task_struct *blocked) {
  //printk("UNBLOCK");
  struct stats *st = get_task_stats(blocked);
  /*char buff[24];
  itoa(get_ticks(),buff);
  printk(buff);
  itoa(st->elapsed_total_ticks,buff);
  printk(buff);*/
  struct list_head *l = get_task_list(blocked);
  
  update_process_state(blocked, &readyqueue);
  st->blocked_ticks += (get_ticks()-st->elapsed_total_ticks);
  st->elapsed_total_ticks = get_ticks();
  if (needs_sched()) {
    update_process_state(current(), &readyqueue);
    sched_next();
  }
}

