#include "types.h"
#include "stat.h"
#include "user.h"

#define NTHREADS 3
#define STACKSIZE 4096
#define ITERATIONS 100000

// One page per thread, aligned so clone() can validate each stack page.
static char stacks[NTHREADS][STACKSIZE] __attribute__((aligned(4096)));
static int worker_ids[NTHREADS];
static volatile int counter;

static void
worker(void *arg)
{
  int id = *(int*)arg;
  int i, old, delay;

  for(i = 0; i < ITERATIONS; i++){
    old = counter;
    // Widen the load/store window so the unsynchronized race is observable
    // on the two-CPU xv6 run used for the screenshot.
    for(delay = 0; delay < 500; delay++)
      __asm__ volatile("nop");
    counter = old + 1;
  }
  printf(1, "thread %d done\n", id);
}

int
main(void)
{
  int i, pid;

  printf(1, "Q1: 3 threads x %d unsynchronized increments\n", ITERATIONS);
  for(i = 0; i < NTHREADS; i++){
    worker_ids[i] = i;
    pid = clone(worker, &worker_ids[i], stacks[i]);
    if(pid < 0){
      printf(1, "clone failed for thread %d\n", i);
      exit();
    }
    printf(1, "created thread %d pid=%d\n", i, pid);
  }
  for(i = 0; i < NTHREADS; i++)
    if(join() < 0){
      printf(1, "join failed\n");
      exit();
    }

  printf(1, "Q1 result: counter=%d (expected 300000 without lost updates)\n",
         counter);
  exit();
}
