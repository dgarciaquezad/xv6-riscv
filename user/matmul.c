#include "kernel/types.h"
#include "user/user.h"

#define N 64
#define REPS 1000

static int a[N][N], b[N][N], c[N][N];

int
main(void)
{
  int i, j, k, r, sum, checksum = 0;
  int start = uptime();

  for(i = 0; i < N; i++)
    for(j = 0; j < N; j++){
      a[i][j] = (i + j) % 7;
      b[i][j] = (i * j) % 5;
    }

  for(r = 0; r < REPS; r++)
    for(i = 0; i < N; i++)
      for(j = 0; j < N; j++){
        sum = 0;
        for(k = 0; k < N; k++)
          sum += a[i][k] * b[k][j];
        c[i][j] = sum;
      }

  for(i = 0; i < N; i++)
    for(j = 0; j < N; j++)
      checksum += c[i][j];

  printf("Time: %d ticks, checksum: %d\n", uptime() - start, checksum);
  exit(0);
}
