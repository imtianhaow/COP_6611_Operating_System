#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int pid;
  int before;
  int after;
  int start;
  int sleep_before;
  int sleep_after;

  pid = getpid();

  before = ticks_running(pid);

  // busy work for 20 ticks
  start = uptime();
  while(uptime() - start < 20);

  after = ticks_running(pid);

  printf(1, "pid: %d\n", pid);
  printf(1, "before busy work: %d\n", before);
  printf(1, "after busy work: %d\n", after);

  // test for a non-existent pid
  printf(1, "missing pid: %d\n",
         ticks_running(99999));

  exit();
}