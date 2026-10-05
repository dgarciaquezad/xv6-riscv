#ifndef XV6_PSTAT_H
#define XV6_PSTAT_H
struct rusage {
  uint cputime;
};
enum procstate {
  UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE
};

struct pstat {
  int pid;
  enum procstate state;
  uint64 size;
  int ppid;
  int priority;             // Base priority
  char name[16];
};
#endif
