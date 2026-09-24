#include "kernel/types.h"
#include "kernel/pstat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int start, elapsed, pid, status, percent;
  struct rusage usage;

  if (argc < 2) {
    fprintf(2, "usage: time command [arguments...]\n");
    exit(1);
  }

  start = uptime();
  pid = fork();

  if (pid < 0) {
    fprintf(2, "time: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    exec(argv[1], &argv[1]);
    fprintf(2, "time: cannot execute %s\n", argv[1]);
    exit(1);
  }

  if (wait2(&status, &usage) < 0) {
    fprintf(2, "time: wait2 failed\n");
    exit(1);
  }

  elapsed = uptime() - start;
  percent = elapsed > 0 ? 100 * usage.cputime / elapsed : 0;
  printf("elapsed time: %d ticks, cpu time: %d ticks, %d%% CPU\n",
         elapsed, usage.cputime, percent);
  exit(0);
}
