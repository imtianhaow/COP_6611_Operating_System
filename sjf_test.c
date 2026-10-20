#include "types.h"
#include "stat.h"
#include "user.h"

#define NCHILD 3
#define WORK 10000000

int
main(int argc, char *argv[])
{
  int i;
  int pid;
  int job;
  int start_time;
  int start_ticks;
  int end_time;
  int end_ticks;
  volatile int j;

  for(i = 0; i < NCHILD; i++){
    pid = fork();

    if(pid < 0){
      printf(2, "sjf_test: fork failed\n");
      exit();
    }

    if(pid == 0){
      // Each child sleeps for 20 ticks to ensure that they all start at roughly the same time.
      sleep(20);

      pid = getpid();
      job = sjf_job_length(pid);
      start_time = uptime();
      start_ticks = ticks_running(pid);

      printf(1, "START pid %d job %d\n", pid, job);

      // Simulate work by performing a busy-wait loop for a number of iterations defined by WORK.
      for(j = 0; j < WORK; j++)
        ;

      end_time = uptime();
      end_ticks = ticks_running(pid);

      printf(1,
        "DONE pid %d job %d elapsed %d cpu %d\n",
        pid,
        job,
        end_time - start_time,
        end_ticks - start_ticks);

      exit();
    }
  }

  for(i = 0; i < NCHILD; i++)
    wait();

  exit();
}