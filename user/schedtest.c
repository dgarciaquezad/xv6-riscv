#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int gate[2];
  int ready[2];
  int priorities[3] = {10, 30, 20};
  int status;
  char token = 'x';

  if (setpriority(49) < 0 ||
      pipe(gate) < 0 || pipe(ready) < 0) {
    fprintf(2, "schedtest: setup failed\n");
    exit(1);
  }

  for (int i = 0; i < 3; i++) {
    int pid = fork();

    if (pid < 0) {
      fprintf(2, "schedtest: fork failed\n");
      exit(1);
    }

    if (pid == 0) {
      close(gate[1]);
      close(ready[0]);

      if (setpriority(priorities[i]) < 0)
        exit(1);

      if (write(ready[1], &token, 1) != 1)
        exit(1);
      close(ready[1]);

      if (read(gate[0], &token, 1) != 1)
        exit(1);
      close(gate[0]);

      printf("Child running: priority %d\n", getpriority());
      exit(0);
    }
  }

  close(gate[0]);
  close(ready[1]);

  for (int i = 0; i < 3; i++) {
    if (read(ready[0], &token, 1) != 1) {
      fprintf(2, "schedtest: readiness failed\n");
      exit(1);
    }
  }
  close(ready[0]);

  printf("Expected order with priority scheduler: 30, 20, 10\n");

  if (write(gate[1], "xxx", 3) != 3)
    exit(1);
  close(gate[1]);

  for (int i = 0; i < 3; i++) {
    if (wait(&status) < 0 || status != 0) {
      fprintf(2, "schedtest: child failed\n");
      exit(1);
    }
  }

  printf("All children finished\n");
  exit(0);
}
