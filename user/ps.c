#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/pstat.h"
#include "user/user.h"

int
main(int argc, char **argv)
{
  struct pstat uproc[NPROC];
  int nprocs;
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

  printf("pid\tstate\t\tsize\tpriority\tppid\tname\n");

  for (int i = 0; i < nprocs; i++) {
    state = "unknown ";
    if (uproc[i].state >= UNUSED &&
        uproc[i].state <= ZOMBIE)
      state = states[uproc[i].state];

    printf("%d\t%s\t%ld\t%d\t\t%d\t%s\n",
           uproc[i].pid, state, (long)uproc[i].size,
           uproc[i].priority, uproc[i].ppid,
           uproc[i].name);
  }

  exit(0);
}





