#include "kernel/types.h"
#include "user/user.h"

void
check(int condition, char *message)
{
  if (!condition) {
    printf("FAIL: %s\n", message);
    exit(1);
  }
  printf("PASS: %s\n", message);
}

int
main(void)
{
  int pid;
  int status;
  char *args[] = {"ps", 0};

  printf("Initial priority: %d\n", getpriority());

  check(setpriority(0) == 0 && getpriority() == 0,
        "priority 0 accepted");

  check(setpriority(49) == 0 && getpriority() == 49,
        "priority 49 accepted");

  check(setpriority(10) == 0 && getpriority() == 10,
        "priority changed to 10");

  check(setpriority(-1) == -1 && getpriority() == 10,
        "priority -1 rejected without changing priority");

  check(setpriority(50) == -1 && getpriority() == 10,
        "priority 50 rejected without changing priority");

  pid = fork();
  check(pid >= 0, "fork succeeded");

  if (pid == 0) {
    check(getpriority() == 10,
          "child inherited priority 10");

    check(setpriority(20) == 0 && getpriority() == 20,
          "child changed its priority to 20");

    printf("ps should show parent at 10 and child at 20:\n");
    exec("ps", args);

    fprintf(2, "FAIL: exec ps\n");
    exit(1);
  }

  check(wait(&status) == pid && status == 0,
        "child and ps finished successfully");

  check(getpriority() == 10,
        "parent priority remained 10");

  printf("All priority tests passed.\n");
  exit(0);
}






