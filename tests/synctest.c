#include "types.h"
#include "stat.h"
#include "user.h"

#define NTHREADS 3
#define STACKSIZE 4096
#define ITERATIONS 100000
#define BUFSIZE 5
#define ITEMS 12

static char stacks[NTHREADS + 2][STACKSIZE] __attribute__((aligned(4096)));
static int worker_ids[NTHREADS];
static volatile int counter;
static int buffer[BUFSIZE];
static int head, tail;

static void
counter_worker(void *arg)
{
  int i;

  (void)arg;
  for(i = 0; i < ITERATIONS; i++){
    if(sem_wait(0) < 0){
      printf(1, "sem_wait(counter) failed\n");
      return;
    }
    counter++;
    sem_signal(0);
  }
}

static void
producer(void *arg)
{
  int i;

  (void)arg;
  for(i = 1; i <= ITEMS; i++){
    sem_wait(1);              // empty slots
    sem_wait(3);              // buffer mutex
    buffer[tail] = i;
    tail = (tail + 1) % BUFSIZE;
    printf(1, "produced %d\n", i);
    sem_signal(3);
    sem_signal(2);            // full slots
  }
}

static void
consumer(void *arg)
{
  int i, item;

  (void)arg;
  for(i = 0; i < ITEMS; i++){
    sem_wait(2);              // full slots
    sem_wait(3);              // buffer mutex
    item = buffer[head];
    head = (head + 1) % BUFSIZE;
    printf(1, "consumed %d\n", item);
    sem_signal(3);
    sem_signal(1);            // empty slots
  }
}

int
main(void)
{
  int i;

  if(sem_init(0, 1) < 0){
    printf(1, "sem_init failed\n");
    exit();
  }
  printf(1, "Q2: protected counter, %d threads x %d increments\n",
         NTHREADS, ITERATIONS);
  for(i = 0; i < NTHREADS; i++){
    worker_ids[i] = i;
    if(clone(counter_worker, &worker_ids[i], stacks[i]) < 0){
      printf(1, "clone failed\n");
      exit();
    }
  }
  for(i = 0; i < NTHREADS; i++)
    if(join() < 0){
      printf(1, "join failed\n");
      exit();
    }
  printf(1, "Q2 counter result: %d (expected 300000)\n", counter);

  if(sem_init(1, BUFSIZE) < 0 || sem_init(2, 0) < 0 || sem_init(3, 1) < 0){
    printf(1, "producer-consumer sem_init failed\n");
    exit();
  }
  head = tail = 0;
  if(clone(producer, 0, stacks[NTHREADS]) < 0 ||
     clone(consumer, 0, stacks[NTHREADS + 1]) < 0){
    printf(1, "producer-consumer clone failed\n");
    exit();
  }
  if(join() < 0 || join() < 0){
    printf(1, "producer-consumer join failed\n");
    exit();
  }
  printf(1, "Q2 producer-consumer complete: %d items, buffer size %d\n",
         ITEMS, BUFSIZE);
  exit();
}
