#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/pstat.h"
#include "user/user.h"

int
main(int argc, char **argv)
{
  struct pstat uproc[NPROC];
  int nprocs;
  uint now;
  char *state;

  static char *states[] = {
    [UNUSED]   = "unused  ",
    [USED]     = "used    ",
    [SLEEPING] = "sleeping",
    [RUNNABLE] = "runnable",
    [RUNNING]  = "running ",
    [ZOMBIE]   = "zombie  "
  };

  nprocs = getprocs(uproc);
  if (nprocs < 0) {
    fprintf(2, "ps: getprocs failed\n");
    exit(1);
  }

  now = (uint)uptime();

  printf("pid\tstate\t\tsize\tage\tpriority\tppid\tname\n");

  for (int i = 0; i < nprocs; i++) {
    state = "unknown ";
    if (uproc[i].state >= UNUSED &&
        uproc[i].state <= ZOMBIE)
      state = states[uproc[i].state];

    printf("%d\t%s\t%ld\t",
           uproc[i].pid, state, (long)uproc[i].size);

    if (uproc[i].state == RUNNABLE) {
      uint age = now - uproc[i].readytime;
      printf("%d", (int)age);
    }

    printf("\t%d\t\t%d\t%s\n",
           uproc[i].priority, uproc[i].ppid,
           uproc[i].name);
  }

  exit(0);
}




