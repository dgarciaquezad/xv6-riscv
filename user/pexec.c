#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int priority = 0;

  if (argc < 3 || argv[1][0] == '\0') {
    fprintf(2, "Usage: pexec priority command [args...]\n");
    exit(1);
  }

  for (int i = 0; argv[1][i] != '\0'; i++) {
    if (argv[1][i] < '0' || argv[1][i] > '9') {
      fprintf(2, "pexec: priority must be 0..49\n");
      exit(1);
    }

    priority = priority * 10 + argv[1][i] - '0';

    if (priority > 49) {
      fprintf(2, "pexec: priority must be 0..49\n");
      exit(1);
    }
  }

  if (setpriority(priority) < 0) {
    fprintf(2, "pexec: setpriority failed\n");
    exit(1);
  }

  exec(argv[2], &argv[2]);
  fprintf(2, "pexec: exec %s failed\n", argv[2]);
  exit(1);
}
