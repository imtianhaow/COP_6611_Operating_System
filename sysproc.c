#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int 
sys_hello(void)
{
  cprintf("Hello from the Kernel!\n");
  return 0;
}


int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// System call to get the number of running ticks for a process
int sys_ticks_running(void) {
  int pid;
  // Get the pid argument from the user space - first argument of the system call
  if(argint(0, &pid) < 0)
    return -1;
  return get_running_ticks(pid);
}

// system call to get the predicted job length for a process
int sys_sjf_job_length(void) {
  int pid;
  // Get the pid argument from the user space - first argument of the system call
  if(argint(0, &pid) < 0)
    return -1;
  return get_sfj_job_length(pid);
}

int sys_set_sched_priority(void) {
  int priority;
  // Get the priority argument from the user space - first argument of the system call
  if(argint(0, &priority) < 0)
    return -1;
  return set_current_priority(priority);
}

int sys_get_sched_priority(void) {
  int pid;
  // Get the pid argument from the user space - first argument of the system call
  if(argint(0, &pid) < 0)
    return -1;
  return get_process_priority(pid);
}

int sys_set_sched_quantum(void) {
  int quantum;
  // Get the quantum argument from the user space - first argument of the system call
  if(argint(0, &quantum) < 0)
    return -1;
  return set_current_time_quantum(quantum);
}