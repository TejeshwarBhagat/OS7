#include "types.h"
#include "stat.h"
#include "user.h"

#define NCHILD 4
#define WORK 200000000
#define CHECK_EVERY 10000000

static int priorities[NCHILD] = {1, 5, 10, 20};

int
main(void)
{
  int i, pid, initial, current, start, finish, next_check;
  int gate[2];
  char release;
  volatile uint work;

  printf(1, "Q4: priority preemption and aging (1=high, 20=low)\n");
  if(pipe(gate) < 0){
    printf(1, "barrier pipe failed\n");
    exit();
  }
  for(i = 0; i < NCHILD; i++){
    pid = fork();
    if(pid < 0){
      printf(1, "fork failed\n");
      exit();
    }
    if(pid == 0){
      close(gate[1]);
      if(read(gate[0], &release, 1) != 1)
        exit();
      close(gate[0]);
      initial = get_priority(getpid());
      printf(1, "start pid=%d priority=%d\n", getpid(), initial);
      start = uptime();
      next_check = CHECK_EVERY;
      for(work = 0; work < WORK; work++){
        if(work == next_check){
          current = get_priority(getpid());
          if(current != initial){
            printf(1, "child pid=%d priority aged %d->%d\n",
                   getpid(), initial, current);
            initial = current;
          }
          next_check += CHECK_EVERY;
        }
      }
      finish = uptime();
      printf(1, "complete pid=%d priority=%d ticks=%d\n",
             getpid(), get_priority(getpid()), finish - start);
      exit();
    }
    if(set_priority(pid, priorities[i]) < 0){
      printf(1, "set_priority failed for pid=%d\n", pid);
      exit();
    }
    printf(1, "forked pid=%d priority=%d\n", pid, get_priority(pid));
  }
  close(gate[0]);
  release = 'x';
  for(i = 0; i < NCHILD; i++)
    if(write(gate[1], &release, 1) != 1){
      printf(1, "barrier release failed\n");
      exit();
    }
  close(gate[1]);
  for(i = 0; i < NCHILD; i++)
    wait();
  printf(1, "Q4 complete: all children finished; age events appear above\n");
  exit();
}
