#include "types.h"
#include "stat.h"
#include "user.h"

#define NCHILD 4
#define WORK 15000000

volatile int result;

int priorities[NCHILD] = {1, 0, 1, 2};

int
main(int argc, char *argv[])
{
  int i;
  int pid;
  int priority;
  int start;
  int before;
  int after;
  volatile int j;

  for(i = 0; i < NCHILD; i++){
    pid = fork();

    if(pid < 0){
      printf(2, "priority_test: fork failed\n");
      exit();
    }

    if(pid == 0){
      if(set_sched_priority(priorities[i]) < 0){
        printf(2, "cannot set priority\n");
        exit();
      }

      if(set_sched_quantum(10) < 0){
        printf(2, "cannot set quantum\n");
        exit();
      }

      // Sleep for 20 ticks to allow all child processes to be created before starting the work
      sleep(20);

      pid = getpid();
      priority = get_sched_priority(pid);
      start = uptime();
      before = ticks_running(pid);

      printf(1, "START pid %d priority %d\n",
             pid, priority);

      result = 0;
      for(j = 0; j < WORK; j++)
        result++;

      after = ticks_running(pid);

      printf(1,
        "DONE pid %d priority %d wall %d cpu %d\n",
        pid,
        priority,
        uptime() - start,
        after - before);

      exit();
    }
  }

  for(i = 0; i < NCHILD; i++)
    wait();

  printf(1, "invalid pid result: %d\n",
         get_sched_priority(99999));

  exit();
}