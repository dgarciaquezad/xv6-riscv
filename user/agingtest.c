#include "kernel/types.h"
#include "user/user.h"

struct result {
  int priority;
  uint response;
  uint turnaround;
  uint checksum;
};

int
main(int argc, char *argv[])
{
  int gate[2], ready[2], results[2];
  int priorities[2] = {0, 49};
  int chunks = 400;
  char token = 'x';
  uint start;
  struct result r[2];

  if (argc > 1)
    chunks = atoi(argv[1]);

  if (chunks < 1 || chunks > 2000) {
    fprintf(2, "Usage: agingtest [chunks: 1..2000]\n");
    exit(1);
  }

  if (setpriority(49) < 0 ||
      pipe(gate) < 0 ||
      pipe(ready) < 0 ||
      pipe(results) < 0) {
    fprintf(2, "agingtest: setup failed\n");
    exit(1);
  }

  for (int i = 0; i < 2; i++) {
    int pid = fork();

    if (pid < 0) {
      fprintf(2, "agingtest: fork failed\n");
      exit(1);
    }

    if (pid == 0) {
      close(gate[1]);
      close(ready[0]);
      close(results[0]);

      if (setpriority(priorities[i]) < 0)
        exit(1);

      if (write(ready[1], &token, 1) != 1)
        exit(1);
      close(ready[1]);

      uint release;
      if (read(gate[0], &release, sizeof(release))
          != sizeof(release))
        exit(1);
      close(gate[0]);

      struct result out;
      out.priority = getpriority();
      out.response = (uint)uptime() - release;

      // Fixed CPU work; volatile prevents its removal.
      volatile uint value = 1;
      int work = (i == 0) ? 3 : chunks;

      for (int j = 0; j < work; j++) {
        for (int k = 0; k < 1000000; k++)
          value = value * 1664525U + 1013904223U;
      }

      out.turnaround = (uint)uptime() - release;
      out.checksum = value;

      if (write(results[1], &out, sizeof(out)) != sizeof(out))
        exit(1);
      close(results[1]);
      exit(0);
    }
  }

  close(gate[0]);
  close(ready[1]);
  close(results[1]);

  for (int i = 0; i < 2; i++) {
    if (read(ready[0], &token, 1) != 1)
      exit(1);
  }
  close(ready[0]);

  printf("CPU workload: low=3 chunks, high=%d chunks\n", chunks);

  start = (uint)uptime();
  for (int i = 0; i < 2; i++) {
    if (write(gate[1], &start, sizeof(start)) != sizeof(start))
      exit(1);
  }
  close(gate[1]);

  for (int i = 0; i < 2; i++) {
    if (read(results[0], &r[i], sizeof(r[i])) != sizeof(r[i]))
      exit(1);
  }
  close(results[0]);

  for (int i = 0; i < 2; i++) {
    int status;
    if (wait(&status) < 0 || status != 0) {
      fprintf(2, "agingtest: child failed\n");
      exit(1);
    }
  }

  printf("priority  response  turnaround (ticks)\n");
  for (int i = 0; i < 2; i++) {
    printf("%d         %d         %d\n",
           r[i].priority, (int)r[i].response,
           (int)r[i].turnaround);
  }

  uint response_sum = r[0].response + r[1].response;
  uint turnaround_sum = r[0].turnaround + r[1].turnaround;

  printf("Average response: %d.%d ticks\n",
         (int)(response_sum / 2),
         (int)((response_sum % 2) * 5));
  printf("Average turnaround: %d.%d ticks\n",
         (int)(turnaround_sum / 2),
         (int)((turnaround_sum % 2) * 5));

  exit(0);
}
